#include "web_server.h"
#include "esp_log.h"
#include "cJSON.h"
#include <string.h>
#include "dashboard.h"  // Include generated HTML

static const char *TAG = "WEB_SERVER";
static httpd_handle_t server = NULL;

// External references
extern int current_soil_moisture;
extern bool current_water_tank_full;
extern bool current_fertilizer_tank_full;
extern bool pump1_running;
extern bool pump2_running;
extern bool auto_mode;
extern bool pump1_manual;
extern bool pump2_manual;
extern int soil_dry_threshold;
extern int pump_duration_ms;
extern int fertilizer_duration_ms;
extern int check_interval_ms;

// External function
extern void control_pump(gpio_num_t relay_pin, bool state);

// HTML Dashboard (now from dashboard.h)
// To update: Edit dashboard.html, then run: python html_to_header.py

// HTTP GET handler for root
static esp_err_t root_handler(httpd_req_t *req)
{
    httpd_resp_set_type(req, "text/html");
    httpd_resp_send(req, dashboard_html, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

// API handler for sensor data (replaces WebSocket)
static esp_err_t api_data_handler(httpd_req_t *req)
{
    // Send sensor data as JSON
    cJSON *root = cJSON_CreateObject();
    cJSON_AddNumberToObject(root, "soil_moisture", current_soil_moisture);
    cJSON_AddBoolToObject(root, "water_tank", current_water_tank_full);
    cJSON_AddBoolToObject(root, "fert_tank", current_fertilizer_tank_full);
    cJSON_AddBoolToObject(root, "pump1", pump1_running);
    cJSON_AddBoolToObject(root, "pump2", pump2_running);
    cJSON_AddBoolToObject(root, "auto_mode", auto_mode);
    cJSON_AddNumberToObject(root, "threshold", soil_dry_threshold);
    cJSON_AddNumberToObject(root, "pump_duration", pump_duration_ms);
    cJSON_AddNumberToObject(root, "fert_duration", fertilizer_duration_ms);
    cJSON_AddNumberToObject(root, "interval", check_interval_ms);
    
    char *json_str = cJSON_PrintUnformatted(root);
    cJSON_Delete(root);

    httpd_resp_set_type(req, "application/json");
    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
    httpd_resp_sendstr(req, json_str);
    
    free(json_str);
    return ESP_OK;
}

// API handler for pump control
static esp_err_t api_pump_handler(httpd_req_t *req)
{
    char buf[100];
    int ret = httpd_req_recv(req, buf, sizeof(buf) - 1);
    if (ret <= 0) {
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }
    buf[ret] = '\0';

    cJSON *json = cJSON_Parse(buf);
    if (json == NULL) {
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }

    int pump = cJSON_GetObjectItem(json, "pump")->valueint;
    bool state = cJSON_GetObjectItem(json, "state")->valueint;

    if (pump == 1) {
        // Set manual control flag when turning ON, clear when turning OFF
        if (state) {
            pump1_manual = true;  // Enable manual mode
            ESP_LOGI(TAG, "Pump 1 MANUAL ON - automatic control disabled");
        } else {
            pump1_manual = false; // Disable manual mode (return to auto)
            ESP_LOGI(TAG, "Pump 1 MANUAL OFF - automatic control re-enabled");
        }
        control_pump(RELAY_PUMP1, state);
        pump1_running = state;
    } else if (pump == 2) {
        // Set manual control flag when turning ON, clear when turning OFF
        if (state) {
            pump2_manual = true;  // Enable manual mode
            ESP_LOGI(TAG, "Pump 2 MANUAL ON - automatic control disabled");
        } else {
            pump2_manual = false; // Disable manual mode (return to auto)
            ESP_LOGI(TAG, "Pump 2 MANUAL OFF - automatic control re-enabled");
        }
        control_pump(RELAY_PUMP2, state);
        pump2_running = state;
    }

    cJSON_Delete(json);

    httpd_resp_set_type(req, "application/json");
    httpd_resp_sendstr(req, "{\"status\":\"ok\"}");
    return ESP_OK;
}

// API handler for auto mode
static esp_err_t api_auto_handler(httpd_req_t *req)
{
    char buf[100];
    int ret = httpd_req_recv(req, buf, sizeof(buf) - 1);
    if (ret <= 0) {
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }
    buf[ret] = '\0';

    cJSON *json = cJSON_Parse(buf);
    if (json == NULL) {
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }

    auto_mode = cJSON_GetObjectItem(json, "enabled")->valueint;
    ESP_LOGI(TAG, "Auto mode: %s", auto_mode ? "ENABLED" : "DISABLED");

    cJSON_Delete(json);

    httpd_resp_set_type(req, "application/json");
    httpd_resp_sendstr(req, "{\"status\":\"ok\"}");
    return ESP_OK;
}

// API handler for settings
static esp_err_t api_settings_handler(httpd_req_t *req)
{
    char buf[200];
    int ret = httpd_req_recv(req, buf, sizeof(buf) - 1);
    if (ret <= 0) {
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }
    buf[ret] = '\0';

    cJSON *json = cJSON_Parse(buf);
    if (json == NULL) {
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }

    soil_dry_threshold = cJSON_GetObjectItem(json, "threshold")->valueint;
    pump_duration_ms = cJSON_GetObjectItem(json, "pump_duration")->valueint;
    fertilizer_duration_ms = cJSON_GetObjectItem(json, "fert_duration")->valueint;
    check_interval_ms = cJSON_GetObjectItem(json, "interval")->valueint;

    ESP_LOGI(TAG, "Settings updated - Threshold: %d, Pump: %d ms, Fert: %d ms, Interval: %d ms",
             soil_dry_threshold, pump_duration_ms, fertilizer_duration_ms, check_interval_ms);

    cJSON_Delete(json);

    httpd_resp_set_type(req, "application/json");
    httpd_resp_sendstr(req, "{\"status\":\"ok\"}");
    return ESP_OK;
}

httpd_handle_t start_webserver(void)
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.uri_match_fn = httpd_uri_match_wildcard;

    if (httpd_start(&server, &config) == ESP_OK) {
        httpd_uri_t root_uri = {
            .uri = "/",
            .method = HTTP_GET,
            .handler = root_handler
        };
        httpd_register_uri_handler(server, &root_uri);

        httpd_uri_t api_data_uri = {
            .uri = "/api/data",
            .method = HTTP_GET,
            .handler = api_data_handler
        };
        httpd_register_uri_handler(server, &api_data_uri);

        httpd_uri_t api_pump_uri = {
            .uri = "/api/pump",
            .method = HTTP_POST,
            .handler = api_pump_handler
        };
        httpd_register_uri_handler(server, &api_pump_uri);

        httpd_uri_t api_auto_uri = {
            .uri = "/api/auto",
            .method = HTTP_POST,
            .handler = api_auto_handler
        };
        httpd_register_uri_handler(server, &api_auto_uri);

        httpd_uri_t api_settings_uri = {
            .uri = "/api/settings",
            .method = HTTP_POST,
            .handler = api_settings_handler
        };
        httpd_register_uri_handler(server, &api_settings_uri);

        ESP_LOGI(TAG, "✅ Web server started successfully");
        return server;
    }

    ESP_LOGE(TAG, "❌ Failed to start web server");
    return NULL;
}

void stop_webserver(httpd_handle_t server)
{
    if (server) {
        httpd_stop(server);
    }
}

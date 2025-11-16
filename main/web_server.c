#include "web_server.h"
#include "esp_log.h"
#include "cJSON.h"
#include <string.h>

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

// HTML Dashboard
static const char* html_page = 
"<!DOCTYPE html>"
"<html><head><meta charset='UTF-8'><meta name='viewport' content='width=device-width,initial-scale=1'>"
"<title>Smart Irrigation Dashboard</title>"
"<style>"
"*{margin:0;padding:0;box-sizing:border-box}"
"body{font-family:'Segoe UI',Arial,sans-serif;background:linear-gradient(135deg,#667eea 0%,#764ba2 100%);min-height:100vh;padding:20px}"
"h1{text-align:center;color:#fff;font-size:2.5em;margin-bottom:30px;text-shadow:2px 2px 4px rgba(0,0,0,0.3)}"
"h2{color:#4CAF50;border-bottom:3px solid #4CAF50;padding-bottom:10px;margin-bottom:20px}"
".container{max-width:1200px;margin:0 auto}"
".card{background:#fff;border-radius:15px;padding:25px;margin:20px 0;box-shadow:0 10px 30px rgba(0,0,0,0.3)}"
".sensor-grid{display:grid;grid-template-columns:repeat(auto-fit,minmax(250px,1fr));gap:20px}"
".sensor{background:linear-gradient(135deg,#667eea 0%,#764ba2 100%);padding:20px;border-radius:12px;text-align:center;color:#fff;box-shadow:0 5px 15px rgba(0,0,0,0.2)}"
".sensor h3{margin-top:0;font-size:1.2em;opacity:0.9}"
".value{font-size:3em;font-weight:bold;margin:15px 0}"
".status{padding:10px 20px;border-radius:25px;font-weight:bold;display:inline-block;margin-top:10px}"
".status.ok{background:#4CAF50}.status.warning{background:#ff9800}.status.error{background:#f44336}"
"button{padding:15px 30px;margin:8px;border:none;border-radius:8px;cursor:pointer;font-size:16px;font-weight:bold;transition:all 0.3s;box-shadow:0 4px 6px rgba(0,0,0,0.1)}"
"button:hover{transform:translateY(-2px);box-shadow:0 6px 12px rgba(0,0,0,0.15)}"
".btn-primary{background:#4CAF50;color:#fff}.btn-primary:hover{background:#45a049}"
".btn-danger{background:#f44336;color:#fff}.btn-danger:hover{background:#da190b}"
".btn-secondary{background:#2196F3;color:#fff}.btn-secondary:hover{background:#0b7dda}"
".control-group{margin:20px 0}"
"label{display:block;margin:15px 0 8px;font-weight:600;color:#333}"
"input[type='number'],input[type='text']{width:100%;padding:12px;border:2px solid #ddd;border-radius:8px;font-size:16px;transition:border 0.3s}"
"input:focus{outline:none;border-color:#667eea}"
".pump-control{display:flex;justify-content:space-around;flex-wrap:wrap;gap:20px}"
".pump-item{flex:1;min-width:250px;text-align:center}"
".switch{position:relative;display:inline-block;width:60px;height:34px}"
".switch input{opacity:0;width:0;height:0}"
".slider{position:absolute;cursor:pointer;top:0;left:0;right:0;bottom:0;background-color:#ccc;transition:.4s;border-radius:34px}"
".slider:before{position:absolute;content:'';height:26px;width:26px;left:4px;bottom:4px;background-color:white;transition:.4s;border-radius:50%}"
"input:checked+.slider{background-color:#4CAF50}"
"input:checked+.slider:before{transform:translateX(26px)}"
"@media (max-width:768px){.sensor-grid{grid-template-columns:1fr}.pump-control{flex-direction:column}}"
"</style></head><body>"
"<div class='container'>"
"<h1>🌱 Smart Irrigation System</h1>"
"<div class='card'><h2>📊 Real-Time Monitoring</h2><div class='sensor-grid'>"
"<div class='sensor'><h3>💧 Soil Moisture</h3><div class='value' id='soilValue'>--</div>"
"<div class='status' id='soilStatus'>Loading...</div></div>"
"<div class='sensor'><h3>🚰 Water Tank</h3><div class='value' id='waterTank'>--</div>"
"<div class='status' id='waterStatus'>Loading...</div></div>"
"<div class='sensor'><h3>🧪 Fertilizer Tank</h3><div class='value' id='fertTank'>--</div>"
"<div class='status' id='fertStatus'>Loading...</div></div></div></div>"
"<div class='card'><h2>🎛️ Control Panel</h2>"
"<div class='control-group' style='text-align:center'>"
"<label style='display:inline-flex;align-items:center;gap:10px;font-size:1.2em'>"
"<span>Automatic Mode:</span>"
"<label class='switch'><input type='checkbox' id='autoMode' onchange='toggleAuto()' checked>"
"<span class='slider'></span></label></label></div>"
"<div class='pump-control'>"
"<div class='pump-item'><h3>💧 Water Pump</h3>"
"<button class='btn-primary' onclick='controlPump(1,true)'>▶ Turn ON</button>"
"<button class='btn-danger' onclick='controlPump(1,false)'>⏹ Turn OFF</button>"
"<div style='margin-top:15px'><span style='font-weight:bold'>Status:</span> "
"<span class='status' id='pump1Status' style='margin-left:10px'>OFF</span></div></div>"
"<div class='pump-item'><h3>🧪 Fertilizer Pump</h3>"
"<button class='btn-primary' onclick='controlPump(2,true)'>▶ Turn ON</button>"
"<button class='btn-danger' onclick='controlPump(2,false)'>⏹ Turn OFF</button>"
"<div style='margin-top:15px'><span style='font-weight:bold'>Status:</span> "
"<span class='status' id='pump2Status' style='margin-left:10px'>OFF</span></div></div></div></div>"
"<div class='card'><h2>⚙️ System Settings</h2>"
"<div class='control-group'><label>🌡️ Soil Moisture Threshold (ADC Value):</label>"
"<input type='number' id='threshold' placeholder='2800'></div>"
"<div class='control-group'><label>⏱️ Water Pump Duration (milliseconds):</label>"
"<input type='number' id='pumpDuration' placeholder='3000'></div>"
"<div class='control-group'><label>⏱️ Fertilizer Pump Duration (milliseconds):</label>"
"<input type='number' id='fertDuration' placeholder='1500'></div>"
"<div class='control-group'><label>🔄 Check Interval (milliseconds):</label>"
"<input type='number' id='interval' placeholder='5000'></div>"
"<div style='text-align:center'><button class='btn-secondary' onclick='saveSettings()'>💾 Save Settings</button></div></div></div>"
"<script>"
"function fetchData(){"
"fetch('/api/data').then(r=>r.json()).then(d=>updateUI(d)).catch(e=>console.error('Fetch error:',e))}"
"function updateUI(d){"
"document.getElementById('soilValue').textContent=d.soil_moisture;"
"const isDry=d.soil_moisture>d.threshold;"
"document.getElementById('soilStatus').textContent=isDry?'DRY':'OK';"
"document.getElementById('soilStatus').className='status '+(isDry?'warning':'ok');"
"document.getElementById('waterTank').textContent=d.water_tank?'FULL':'EMPTY';"
"document.getElementById('waterStatus').className='status '+(d.water_tank?'ok':'error');"
"document.getElementById('fertTank').textContent=d.fert_tank?'FULL':'EMPTY';"
"document.getElementById('fertStatus').className='status '+(d.fert_tank?'ok':'error');"
"document.getElementById('pump1Status').textContent=d.pump1?'ON':'OFF';"
"document.getElementById('pump1Status').className='status '+(d.pump1?'ok':'');"
"document.getElementById('pump2Status').textContent=d.pump2?'ON':'OFF';"
"document.getElementById('pump2Status').className='status '+(d.pump2?'ok':'');"
"document.getElementById('autoMode').checked=d.auto_mode;"
"document.getElementById('threshold').placeholder=d.threshold;"
"document.getElementById('pumpDuration').placeholder=d.pump_duration;"
"document.getElementById('fertDuration').placeholder=d.fert_duration;"
"document.getElementById('interval').placeholder=d.interval}"
"function toggleAuto(){"
"fetch('/api/auto',{method:'POST',headers:{'Content-Type':'application/json'},"
"body:JSON.stringify({enabled:document.getElementById('autoMode').checked})})"
".then(r=>r.json()).then(d=>console.log('Auto mode:',d))}"
"function controlPump(pump,state){"
"fetch('/api/pump',{method:'POST',headers:{'Content-Type':'application/json'},"
"body:JSON.stringify({pump:pump,state:state})}).then(r=>r.json()).then(d=>console.log('Pump:',d))}"
"function saveSettings(){"
"const s={threshold:parseInt(document.getElementById('threshold').value)||2800,"
"pump_duration:parseInt(document.getElementById('pumpDuration').value)||3000,"
"fert_duration:parseInt(document.getElementById('fertDuration').value)||1500,"
"interval:parseInt(document.getElementById('interval').value)||5000};"
"fetch('/api/settings',{method:'POST',headers:{'Content-Type':'application/json'},"
"body:JSON.stringify(s)}).then(r=>r.json()).then(d=>{alert('✅ Settings saved successfully!');console.log(d)})}"
"fetchData();setInterval(fetchData,2000);"
"</script></body></html>";

// HTTP GET handler for root
static esp_err_t root_handler(httpd_req_t *req)
{
    httpd_resp_set_type(req, "text/html");
    httpd_resp_send(req, html_page, HTTPD_RESP_USE_STRLEN);
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
        pump1_manual = state;
        control_pump(RELAY_PUMP1, state);
        pump1_running = state;
    } else if (pump == 2) {
        pump2_manual = state;
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

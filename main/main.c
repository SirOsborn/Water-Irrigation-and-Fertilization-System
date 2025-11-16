#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"
#include "esp_log.h"

#include "sensors.h"
#include "irrigation_control.h"
#include "wifi_config.h"
#include "web_server.h"

static const char *TAG = "MAIN";

// Global shared state variables
int soil_dry_threshold = 2800;
int pump_duration_ms = 3000;
int fertilizer_duration_ms = 1500;
int check_interval_ms = 5000;

bool auto_mode = true;
bool pump1_manual = false;
bool pump2_manual = false;

int current_soil_moisture = 0;
bool current_water_tank_full = false;
bool current_fertilizer_tank_full = false;
bool pump1_running = false;
bool pump2_running = false;

void app_main(void)
{
    ESP_LOGI(TAG, "===========================================");
    ESP_LOGI(TAG, "   🌱 Smart Irrigation System with Web UI");
    ESP_LOGI(TAG, "===========================================\n");
    
    // Initialize NVS (required for WiFi)
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    
    // Initialize hardware
    ESP_LOGI(TAG, "📡 Initializing hardware...");
    init_gpio();
    init_adc();
    
    // Initialize WiFi
    ESP_LOGI(TAG, "📶 Connecting to WiFi...");
    wifi_init_sta();
    
    // Start web server
    ESP_LOGI(TAG, "🌐 Starting web server...");
    start_webserver();
    
    ESP_LOGI(TAG, "");
    ESP_LOGI(TAG, "✅ System initialized successfully!");
    ESP_LOGI(TAG, "📱 Access dashboard at: http://<ESP32_IP_ADDRESS>");
    ESP_LOGI(TAG, "   (Check serial monitor for IP address)");
    ESP_LOGI(TAG, "");
    
    // Start irrigation control task
    xTaskCreate(irrigation_task, "irrigation_task", 4096, NULL, 5, NULL);
    
    ESP_LOGI(TAG, "🚀 Irrigation system is now running!");
}

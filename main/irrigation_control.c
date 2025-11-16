#include "irrigation_control.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "IRRIGATION_CTRL";

// External references to shared state
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

void control_pump(gpio_num_t relay_pin, bool state) {
    if (state) {
        gpio_set_level(relay_pin, 0);  // LOW activates relay (active-low)
        ESP_LOGI(TAG, "Pump ON (GPIO %d)", relay_pin);
    } else {
        gpio_set_level(relay_pin, 1);  // HIGH deactivates relay
        ESP_LOGI(TAG, "Pump OFF (GPIO %d)", relay_pin);
    }
}

void control_water_alert_led(bool state) {
    gpio_set_level(ALERT_LED_WATER, state ? 1 : 0);
    if (state) {
        ESP_LOGW(TAG, "🔴 Water Tank LED ON - Tank Empty!");
    } else {
        ESP_LOGI(TAG, "✓ Water Tank LED OFF");
    }
}

void control_fertilizer_alert_led(bool state) {
    gpio_set_level(ALERT_LED_FERT, state ? 1 : 0);
    if (state) {
        ESP_LOGW(TAG, "🔴 Fertilizer Tank LED ON - Tank Empty!");
    } else {
        ESP_LOGI(TAG, "✓ Fertilizer Tank LED OFF");
    }
}

void irrigation_task(void *pvParameters) {
    ESP_LOGI(TAG, "Irrigation system started");
    
    while (1) {
        // Read soil moisture
        int soil_moisture = read_soil_moisture();
        current_soil_moisture = soil_moisture;
        ESP_LOGI(TAG, "Soil Moisture: %d", soil_moisture);
        
        // Read water tank levels
        int water_level_raw = read_water_level_digital(WATER_LEVEL1);
        int fertilizer_level_raw = read_water_level_digital(WATER_LEVEL2);
        
        bool water_tank_full = water_level_raw;
        bool fertilizer_tank_full = fertilizer_level_raw;
        
        current_water_tank_full = water_tank_full;
        current_fertilizer_tank_full = fertilizer_tank_full;
        
        ESP_LOGI(TAG, "Water Tank [Raw: %d]: %s", water_level_raw, water_tank_full ? "HAS WATER" : "EMPTY");
        ESP_LOGI(TAG, "Fertilizer Tank [Raw: %d]: %s", fertilizer_level_raw, fertilizer_tank_full ? "HAS LIQUID" : "EMPTY");
        
        // Control alert LEDs
        control_water_alert_led(!water_tank_full);
        control_fertilizer_alert_led(!fertilizer_tank_full);
        
        // Only run automatic irrigation if auto mode is enabled and no manual control
        if (auto_mode && !pump1_manual && !pump2_manual) {
            // Check if soil is dry
            if (soil_moisture > soil_dry_threshold) {
                ESP_LOGI(TAG, "Soil is DRY (moisture: %d > %d) - Starting irrigation", soil_moisture, soil_dry_threshold);
                
                if (water_tank_full) {
                    ESP_LOGI(TAG, "Pumping water for %d ms", pump_duration_ms);
                    control_pump(RELAY_PUMP1, true);
                    pump1_running = true;
                    vTaskDelay(pdMS_TO_TICKS(pump_duration_ms));
                    control_pump(RELAY_PUMP1, false);
                    pump1_running = false;
                    
                    vTaskDelay(pdMS_TO_TICKS(1000));
                    
                    if (fertilizer_tank_full) {
                        ESP_LOGI(TAG, "Pumping fertilizer for %d ms", fertilizer_duration_ms);
                        control_pump(RELAY_PUMP2, true);
                        pump2_running = true;
                        vTaskDelay(pdMS_TO_TICKS(fertilizer_duration_ms));
                        control_pump(RELAY_PUMP2, false);
                        pump2_running = false;
                    } else {
                        ESP_LOGW(TAG, "Fertilizer tank is EMPTY - skipping");
                    }
                } else {
                    ESP_LOGW(TAG, "Water tank is EMPTY - cannot irrigate");
                }
            } else {
                ESP_LOGI(TAG, "Soil moisture is adequate - no irrigation needed");
            }
        }
        
        ESP_LOGI(TAG, "Waiting %d seconds before next check...\n", check_interval_ms / 1000);
        vTaskDelay(pdMS_TO_TICKS(check_interval_ms));
    }
}

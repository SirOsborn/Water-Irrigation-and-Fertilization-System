#include "sensors.h"
#include "esp_log.h"

static const char *TAG = "SENSORS";
static esp_adc_cal_characteristics_t adc_chars;

void init_gpio(void) {
    // Configure relay pins and LEDs as output
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << RELAY_PUMP1) | (1ULL << RELAY_PUMP2) | (1ULL << ALERT_LED_WATER) | (1ULL << ALERT_LED_FERT),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&io_conf);
    
    // Set relays to OFF (HIGH for active-low relay modules)
    gpio_set_level(RELAY_PUMP1, 1);
    gpio_set_level(RELAY_PUMP2, 1);
    
    // Set alert LEDs to OFF initially
    gpio_set_level(ALERT_LED_WATER, 0);
    gpio_set_level(ALERT_LED_FERT, 0);
    
    // Configure water level sensor pins as input
    gpio_config_t input_conf = {
        .pin_bit_mask = (1ULL << WATER_LEVEL1) | (1ULL << WATER_LEVEL2),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&input_conf);
    
    ESP_LOGI(TAG, "GPIO initialized");
}

void init_adc(void) {
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(SOIL_MOISTURE, ADC_ATTEN_DB_11);
    esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12, 1100, &adc_chars);
    ESP_LOGI(TAG, "ADC initialized");
}

int read_soil_moisture(void) {
    int adc_reading = 0;
    for (int i = 0; i < 10; i++) {
        adc_reading += adc1_get_raw(SOIL_MOISTURE);
    }
    return adc_reading / 10;
}

bool read_water_level_digital(gpio_num_t pin) {
    return gpio_get_level(pin);
}

#ifndef IRRIGATION_CONTROL_H
#define IRRIGATION_CONTROL_H

#include "driver/gpio.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"

// GPIO Pins
#define RELAY_PUMP1     GPIO_NUM_27
#define RELAY_PUMP2     GPIO_NUM_26
#define WATER_LEVEL1    GPIO_NUM_34
#define WATER_LEVEL2    GPIO_NUM_35
#define SOIL_MOISTURE   ADC1_CHANNEL_0
#define ALERT_LED_WATER GPIO_NUM_22
#define ALERT_LED_FERT  GPIO_NUM_23

// Function declarations
void init_gpio(void);
void init_adc(void);
int read_soil_moisture(void);
bool read_water_level_digital(gpio_num_t pin);
void control_pump(gpio_num_t relay_pin, bool state);
void control_water_alert_led(bool state);
void control_fertilizer_alert_led(bool state);
void irrigation_task(void *pvParameters);

#endif // IRRIGATION_CONTROL_H

#ifndef IRRIGATION_CONTROL_H
#define IRRIGATION_CONTROL_H

#include "sensors.h"
#include <stdbool.h>

// Irrigation control functions
void control_pump(gpio_num_t relay_pin, bool state);
void control_water_alert_led(bool state);
void control_fertilizer_alert_led(bool state);
void irrigation_task(void *pvParameters);

#endif // IRRIGATION_CONTROL_H

#ifndef SENSORS_H
#define SENSORS_H

#include "irrigation_control.h"

void init_gpio(void);
void init_adc(void);
int read_soil_moisture(void);
bool read_water_level_digital(gpio_num_t pin);

#endif // SENSORS_H

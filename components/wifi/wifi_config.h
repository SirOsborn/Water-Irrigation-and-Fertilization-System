#ifndef WIFI_CONFIG_H
#define WIFI_CONFIG_H

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_netif.h"

// WiFi Configuration - CHANGE THESE TO YOUR NETWORK
#define WIFI_SSID      "CAMTECH-STUDENT"
#define WIFI_PASS      "2!CamTech!@$"
#define WIFI_MAXIMUM_RETRY  5

#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT      BIT1

void wifi_init_sta(void);

#endif // WIFI_CONFIG_H

#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include "esp_http_server.h"
#include "irrigation_control.h"

httpd_handle_t start_webserver(void);
void stop_webserver(httpd_handle_t server);

#endif // WEB_SERVER_H

idf_component_register(
    SRCS "web_server.c"
    INCLUDE_DIRS "." "../../web"
    REQUIRES esp_http_server json irrigation
)

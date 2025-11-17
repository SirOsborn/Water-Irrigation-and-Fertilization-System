idf_component_register(
    SRCS "irrigation_control.c"
    INCLUDE_DIRS "."
    REQUIRES freertos sensors
)

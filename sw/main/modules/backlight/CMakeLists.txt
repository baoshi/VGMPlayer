add_library(backlight INTERFACE)

target_sources(backlight INTERFACE backlight.c)

target_link_libraries(backlight INTERFACE
    hardware_gpio
    hardware_pwm
)

target_include_directories(backlight INTERFACE ${CMAKE_CURRENT_LIST_DIR})


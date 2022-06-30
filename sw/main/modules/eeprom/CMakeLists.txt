add_library(eeprom INTERFACE)

target_sources(eeprom INTERFACE eeprom.c)

target_link_libraries(eeprom INTERFACE
    pico_stdlib
    hardware_flash
)

target_include_directories(eeprom INTERFACE ${CMAKE_CURRENT_LIST_DIR})


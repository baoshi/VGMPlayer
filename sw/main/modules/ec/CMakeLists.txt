add_library(ec INTERFACE)

target_sources(ec INTERFACE ec.c)

target_link_libraries(ec INTERFACE
    pico_stdlib
    hardware_i2c
)

target_include_directories(ec INTERFACE ${CMAKE_CURRENT_LIST_DIR})


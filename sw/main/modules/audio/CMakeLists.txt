add_library(audio INTERFACE)

target_sources(audio INTERFACE
    wm8978.c
    audio.c
)

pico_generate_pio_header(audio ${CMAKE_CURRENT_LIST_DIR}/i2s.pio)

target_link_libraries(audio INTERFACE
    pico_stdlib
    pico_time
    hardware_gpio
    hardware_i2c
    hardware_dma
    hardware_irq
    hardware_pio
)

target_include_directories(audio INTERFACE ${CMAKE_CURRENT_LIST_DIR})


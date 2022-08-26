add_library(audio INTERFACE)

target_sources(audio INTERFACE
    wm8978.c
    i2s.c
    audio_memory.c
    audio_buffer.c
    audio.c
    direct_file_reader.c
)

pico_generate_pio_header(audio ${CMAKE_CURRENT_LIST_DIR}/i2s.pio)

target_link_libraries(audio INTERFACE
    pico_stdlib
    pico_time
    pico_multicore
    hardware_gpio
    hardware_i2c
    hardware_dma
    hardware_irq
    hardware_pio
    umm_malloc
    fatfs
)

target_include_directories(audio INTERFACE ${CMAKE_CURRENT_LIST_DIR})


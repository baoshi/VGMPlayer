add_library(display INTERFACE)

target_sources(display INTERFACE
    st7789.c
    lvtheme.c
    lvstyle.c
    lvinput.c
    lvsupp.c
    lvfont_helvr14.c
    display.c
)

target_link_libraries(display INTERFACE
    pico_stdlib
    hardware_spi
    hardware_dma
    lvgl
)

target_include_directories(display INTERFACE ${CMAKE_CURRENT_LIST_DIR})


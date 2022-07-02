add_library(display INTERFACE)

target_sources(display INTERFACE
    st7789.c
    lvtheme.c
    lvstyle.c
    lvinput.c
    lvsupp.c
    lvfont_helvr14.c
    browser_icon_folder.c
    browser_icon_file.c
    browser_icon_nsf.c
    settings_icon_brightness.c
    display.c
)

target_compile_definitions(display INTERFACE LV_LVGL_H_INCLUDE_SIMPLE)

target_link_libraries(display INTERFACE
    pico_stdlib
    hardware_spi
    hardware_dma
    lvgl
)

target_include_directories(display INTERFACE ${CMAKE_CURRENT_LIST_DIR})


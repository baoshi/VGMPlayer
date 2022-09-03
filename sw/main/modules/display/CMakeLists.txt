add_library(display INTERFACE)

target_sources(display INTERFACE
    st7789.c
    lvtheme.c
    lvstyle.c
    lvsupp.c
    lvspectrum.c
    img_browser_folder.c
    img_browser_file.c
    img_browser_pageup.c
    img_browser_pagedown.c
    img_browser_nsf.c
    img_popup_brightness.c
    img_popup_speaker.c
    img_popup_headphone.c
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


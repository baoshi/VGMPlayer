add_library(powerman INTERFACE)

target_sources(powerman INTERFACE powerman.c)

target_link_libraries(powerman INTERFACE
    backlight
)

target_include_directories(powerman INTERFACE ${CMAKE_CURRENT_LIST_DIR})


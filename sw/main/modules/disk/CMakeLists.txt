add_library(disk INTERFACE)

target_sources(disk INTERFACE
    sdcard_ll.c
    sdcard_fatfs_io.c
    crc.c
    disk.c
)

target_link_libraries(disk INTERFACE
    pico_stdlib
    fatfs
)

target_include_directories(disk INTERFACE ${CMAKE_CURRENT_LIST_DIR})


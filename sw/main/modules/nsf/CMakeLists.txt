add_library(nsf INTERFACE)

target_sources(nsf INTERFACE
    blip_buf.c
    nesbus.c
    nescpu.c
    nesapu.c
    nesfloat.c
    nsf.c 
    nsfreader_file_fatfs.c
)

target_link_libraries(nsf INTERFACE
    pico_stdlib
    fatfs
)

target_include_directories(nsf INTERFACE ${CMAKE_CURRENT_LIST_DIR})


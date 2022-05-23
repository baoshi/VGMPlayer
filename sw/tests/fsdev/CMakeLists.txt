cmake_minimum_required(VERSION 3.17)

set(CMAKE_VERBOSE_MAKEFILE ON)

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

set(PROJ_NAME "FSDev")
project(${PROJ_NAME} C CXX)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# BSP
set(PICO_BOARD vgmpboard)
set(PICO_BOARD_CMAKE_DIRS ${PROJECT_SOURCE_DIR}/bsp)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# this will propagate to sub directories
include_directories(${PROJECT_SOURCE_DIR})

# 3rd party libraries
add_subdirectory(lib/umm_malloc)
add_subdirectory(lib/fatfs)

# modules
add_subdirectory(modules/disk)

add_executable(${PROJ_NAME}
    my_debug.c
    my_mem.c
    hw_conf.c
    sw_conf.c
    tick.c
    path_utils.c
    lister.c
    main.c
)

target_link_libraries(${PROJ_NAME} 
    pico_stdlib
    hardware_gpio
    hardware_pwm
    hardware_i2c
    umm_malloc
    disk
)

#target_compile_definitions(${PROJ_NAME} PRIVATE PICO_DEBUG_MALLOC=1)
#target_compile_definitions(${PROJ_NAME} PRIVATE PICO_DEBUG_MALLOC_LOW_WATER=1)   

# Custom link script
bsp_set_link_script(${PROJ_NAME})

#pico_add_extra_outputs(${PROJ_NAME})


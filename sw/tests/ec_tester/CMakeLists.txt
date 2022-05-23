# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)


# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(ec_tester C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_executable(ec_tester ec_tester.c )

pico_set_program_name(ec_tester "ec_tester")
pico_set_program_version(ec_tester "0.1")

pico_enable_stdio_uart(ec_tester 1)
pico_enable_stdio_usb(ec_tester 0)

# Add the standard library to the build
target_link_libraries(ec_tester pico_stdlib)

# Add any user requested libraries
target_link_libraries(ec_tester
        hardware_i2c
        )

pico_add_extra_outputs(ec_tester)


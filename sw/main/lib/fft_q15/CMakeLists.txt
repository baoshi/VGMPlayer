add_library(fft_q15 INTERFACE)

target_sources(fft_q15 INTERFACE
    arm_common_tables.c
    fft_window.c
    fft_q15.c
    fpsqrt.c
)

target_include_directories(fft_q15 INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}
)
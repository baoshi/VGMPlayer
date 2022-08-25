set(BOARD_HEADER_FILE ${CMAKE_CURRENT_LIST_DIR}/${PICO_BOARD}.h)
message("Using board configuration header ${BOARD_HEADER_FILE}")
list(APPEND PICO_CONFIG_HEADER_FILES ${BOARD_HEADER_FILE})


function(bsp_set_link_script TARGET)
    set(BOARD_LINKER_SCRIPT ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/rp2040_audio_16m.ld)
    message("Using board link script ${BOARD_LINKER_SCRIPT}")
    set_target_properties(${TARGET} PROPERTIES PICO_TARGET_LINKER_SCRIPT ${BOARD_LINKER_SCRIPT})
    target_link_options(${PROJECT_NAME} PRIVATE "LINKER:--print-memory-usage")
endfunction()

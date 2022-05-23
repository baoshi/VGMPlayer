set(UMM_MALLOC_SRC ${CMAKE_CURRENT_LIST_DIR}/umm_malloc/src)

add_library(umm_malloc INTERFACE)

target_sources(umm_malloc INTERFACE
    ${UMM_MALLOC_SRC}/umm_malloc.c
)

target_include_directories(umm_malloc INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}
    ${UMM_MALLOC_SRC}
)
#pragma once

#include <stdint.h>
#include "file_reader.h"

#ifdef __cplusplus
extern "C" {
#endif

file_reader_t * dfreader_create(const char* fn);

void dfreader_destroy(file_reader_t* cfr);

#ifdef __cplusplus
}
#endif
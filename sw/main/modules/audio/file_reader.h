#pragma once

// General interface for file reader

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif


typedef struct file_reader_s file_reader_t;

struct file_reader_s
{
    file_reader_t *self;
    size_t (*read)(file_reader_t *self, uint8_t *out, size_t offset, size_t length);
    size_t (*size)(file_reader_t *self);
};


#ifdef __cplusplus
}
#endif
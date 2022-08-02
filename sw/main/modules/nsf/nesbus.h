#pragma once

#include <stdint.h>
#include <stdbool.h>


#ifdef __cplusplus
extern "C" {
#endif


typedef struct nesbus_read_handler_s
{
    const char* tag;
    uint16_t lo, hi;
    void* cookie;
    bool (*fn)(uint16_t addr, uint8_t* rval, void *cookie);   // read function, return true if succeeded
} nesbus_read_handler_t;


typedef struct nesbus_write_handler_s
{
    const char* tag;
    uint16_t lo, hi;
    void* cookie;
    bool (*fn)(uint16_t addr, uint8_t val, void *cookie);  // write function, return true if succeeded
} nesbus_write_handler_t;



typedef struct nesbus_ctx_s
{
    nesbus_read_handler_t* read_table;
    int read_table_max, read_table_cur;
    nesbus_write_handler_t* write_table;
    int write_table_max, write_table_cur;
} nesbus_ctx_t;


nesbus_ctx_t* nesbus_create(int max_read, int max_write);
void nesbus_destroy(nesbus_ctx_t* ctx);
bool nesbus_add_read_handler(nesbus_ctx_t* ctx, const char* tag, uint16_t lo, uint16_t hi, bool (*handler)(uint16_t, uint8_t*, void*), void* cookie);
bool nesbus_add_write_handler(nesbus_ctx_t* ctx, const char* tag, uint16_t lo, uint16_t hi, bool (*handler)(uint16_t, uint8_t, void*), void* cookie);
void nesbus_clear_handlers(nesbus_ctx_t* ctx);
void nesbus_dump_handlers(nesbus_ctx_t* ctx);

uint8_t nesbus_read(nesbus_ctx_t* ctx, uint16_t address);
void nesbus_write(nesbus_ctx_t* ctx, uint16_t address, uint8_t value);



#ifdef __cplusplus
}
#endif

#include <string.h>
#include <stdbool.h>
#include "platform.h"
#include "ff.h"
#include "nsfreader_file_fatfs.h"


// NSF File Reader
typedef struct nfr_ctx_s
{
    // super class
    nsfreader_ctx_t super;
    // Private fields
    FIL fd;
    bool fd_valid;
    uint8_t* cache;
    uint32_t cache_size;
    uint32_t cache_data_length;
    uint32_t cache_offset;
#ifdef NFR_MEASURE_CACHE_PERFORMACE
    unsigned long long cache_hit;
    unsigned long long cache_miss;
#endif
} nfr_ctx_t;


static uint32_t read(nsfreader_ctx_t* self, uint8_t* out, uint32_t offset, uint32_t length)
{
    // Simplified implementation. Cache is only considered when reading 1 byte.
    nfr_ctx_t* ctx = (nfr_ctx_t*)self;
    if (length == 0)
        return 0;
    if (length > 1)
    {
        UINT read;
        f_lseek(&(ctx->fd), offset);
        f_read(&(ctx->fd), out, 1, &read);
        return (uint32_t)read;
    }
    if ((offset >= ctx->cache_offset) && (offset < ctx->cache_offset + ctx->cache_data_length))
    {
        // Cache hit
#ifdef NFR_MEASURE_CACHE_PERFORMACE
        ++(ctx->cache_hit);
#endif
        *out = ctx->cache[offset - ctx->cache_offset];
        return 1;
    }
    // Cache miss
#ifdef NFR_MEASURE_CACHE_PERFORMACE
    ++(ctx->cache_miss);
#endif
    f_lseek(&(ctx->fd), offset);
    ctx->cache_offset = offset;
    UINT read;
    f_read(&(ctx->fd), ctx->cache, ctx->cache_size, &read);
    ctx->cache_data_length = (uint32_t)read;
    if (ctx->cache_data_length > 0)
    {
        *out = ctx->cache[0];
        return 1;
    }
    // Offset is out-of-bound
    return 0;
}


static uint32_t size(nsfreader_ctx_t* self)
{
    nfr_ctx_t* ctx = (nfr_ctx_t*)self;
    if (ctx)
    {
        return (uint32_t)f_size(&(ctx->fd));
    }
    return 0;
}


nsfreader_ctx_t* nfr_create(const char* fn, uint32_t cache_size)
{
    FRESULT fr;
    FIL fd; 
    bool fd_valid = false;
    nfr_ctx_t* ctx = 0;

    fr = f_open(&fd, fn, FA_OPEN_EXISTING);
    if (fr != FR_OK)
        goto create_exit;
    fd_valid = true;

    ctx = (nfr_ctx_t*)NSF_MALLOC(sizeof(nfr_ctx_t));
    if (0 == ctx)
        goto create_exit;

    memset(ctx, 0, sizeof(nfr_ctx_t));

    ctx->cache = (uint8_t*)NSF_MALLOC(cache_size);
    if (0 == ctx->cache)
        goto create_exit;

    memcpy(&(ctx->fd), &fd, sizeof(fd));
    ctx->fd_valid = true;
    ctx->cache_size = cache_size;
    ctx->cache_data_length = 0;
    ctx->cache_offset = 0;

    ctx->super.self = (nsfreader_ctx_t*)ctx;
    ctx->super.read = read;
    ctx->super.size = size;

#ifdef NFR_MEASURE_CACHE_PERFORMACE
    ctx->cache_hit = 0;
    ctx->cache_miss = 0;
#endif

    return (nsfreader_ctx_t*)ctx;

create_exit:
    if (ctx && ctx->cache)
        MY_FREE(ctx->cache);
    if (ctx)
        MY_FREE(ctx);
    if (fd_valid)
        f_close(&fd);
    return 0;
}


void nfr_destroy(nsfreader_ctx_t* nfr)
{
    nfr_ctx_t* ctx = (nfr_ctx_t*)nfr;
    if (0 == ctx)
        return;
    if (ctx->cache)
        free(ctx->cache);
    if (ctx->fd_valid)
        f_close(&(ctx->fd));
    free(ctx);
}


#ifdef NFR_MEASURE_CACHE_PERFORMACE

void nfr_show_cache_status(nsfreader_ctx_t* nfr)
{
    nfr_ctx_t* ctx = (nfr_ctx_t*)nfr;
    NSF_PRINTF("NSF: cache status: (%llu/%llu), hit %.1f%%\n", ctx->cache_hit, ctx->cache_hit + ctx->cache_miss, (ctx->cache_hit * 100.0f) / (ctx->cache_hit + ctx->cache_miss));
}

#endif

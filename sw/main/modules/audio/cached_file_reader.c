#include <string.h>
#include <ff.h>
#include "audio.h"
#include "cached_file_reader.h"


// Cached File Reader
typedef struct cfr_s
{
    // super class
    file_reader_t super;
    // private fields
    FIL fd;
    bool fd_valid;
    uint8_t* cache;
    size_t cache_size;
    size_t cache_length;
    size_t cache_offset;
#ifdef CFR_MEASURE_CACHE_PERFORMACE
    unsigned long long cache_hit;
    unsigned long long cache_miss;
#endif
} cfr_t;


static size_t read_direct(cfr_t *ctx, uint8_t *out, size_t offset, size_t length)
{
    size_t read = 0, total = 0;
    if (offset != f_tell(&(ctx->fd)))
        f_lseek(&(ctx->fd), offset);
    while (length > ctx->cache_size)
    {
        f_read(&(ctx->fd), (void *)out, ctx->cache_size, &read);
        if (0 == read) break;
        out += read;
        total += read;
        offset += read;
        length -= read;
    }
    if (length > 0)
    {
        read = 0;
        FRESULT fr = f_read(&(ctx->fd), (void *)(ctx->cache), ctx->cache_size, &read);
        if (read > 0)
        {
            ctx->cache_offset = offset;
            ctx->cache_length = read;
            length = length > read ? read : length;
            memcpy(out, ctx->cache, length);
            total += length;
        }
    }
    return total;
}


/*                                                  off    len   o_c_s   c_s   o_c_l
 *  cache off=4,len=3          |4|5|6|                                     
 *  read (1, 3)          |1|2|3|                     1      3      x      x      x
 *  read (1, 4)          |1|2|3|4|                   1      4      3      0      1
 *  read (4, 1)                |4|                   4      1      0      0      1
 *  read (6, 1)                    |6|               6      1      0      2      1
 *  read (4, 3)                |4|5|6|               4      3      0      0      3
 *  read (2, 6)            |2|3|4|5|6|7|             2      6      2      0      3
 *  read (6, 1)                    |6|               6      1      0      2      1
 *  read (6, 3)                    |6|7|8|           6      3      0      2      1
 *  read (7, 3)                      |7|8|9|         7      3      x      x      x
 */

static size_t read(file_reader_t *self, uint8_t *out, size_t offset, size_t length)
{
    cfr_t *ctx = (cfr_t *)self;
    size_t o_c_s; // out_cached_start
    size_t o_c_l; // out_cached_length
    size_t c_s;   // cached_start
    size_t total = 0, temp;

    if (length == 0)
        return 0;

    if ((size_t)-1 == offset)
        offset = f_tell(&(ctx->fd));

    // Check if reqest data is already in cache (or part of)
    if ((ctx->cache_length > 0) && (offset + length > ctx->cache_offset) && (offset < ctx->cache_offset + ctx->cache_length))
    {
        o_c_s = (offset > ctx->cache_offset) ? 0 : (ctx->cache_offset - offset);
        c_s = offset + o_c_s - ctx->cache_offset;
        if (length - o_c_s > ctx->cache_length - c_s)
            o_c_l = ctx->cache_length - c_s;
        else
            o_c_l = length - o_c_s;
        // transfer from catch to output
        memcpy(out + o_c_s, ctx->cache + c_s, o_c_l);
        total += o_c_l;
#ifdef CFR_MEASURE_CACHE_PERFORMACE
        ctx->cache_hit += o_c_l;
#endif
    }
    else
    {
        // cache miss, fetch fresh data
        temp = read_direct(ctx, out, offset, length);
#ifdef CFR_MEASURE_CACHE_PERFORMACE
        ctx->cache_miss += temp;
#endif
        return temp;
    }
    // fetch data before cached region
    if (o_c_s > 0)
    {
        temp = read_direct(ctx, out, offset, o_c_s);
#ifdef CFR_MEASURE_CACHE_PERFORMACE
        ctx->cache_miss += temp;
#endif
        total += temp;
    }
    // fetch data after cached region
    if (length > o_c_s + o_c_l)
    {
        temp = read_direct(ctx, out + o_c_s + o_c_l, offset + o_c_s + o_c_l, length - o_c_s - o_c_l);
#ifdef CFR_MEASURE_CACHE_PERFORMACE
        ctx->cache_miss += temp;
#endif
        total += temp;        
    }
    
    return total;
}


static size_t size(file_reader_t *self)
{
    cfr_t *ctx = (cfr_t*)self;
    if (ctx && ctx->fd_valid)
    {
        return (size_t)f_size(&(ctx->fd));
    }
    return 0;
}


file_reader_t * cfreader_create(const char* fn, size_t cache_size)
{
    FRESULT fr;
    FIL fd;
    bool fd_valid = false;
    cfr_t *ctx = 0;
    
    do
    {
        fr = f_open(&fd, fn, FA_READ | FA_OPEN_EXISTING);
        if (fr != FR_OK)
            break;
        fd_valid = true;

        ctx = (cfr_t*)audio_malloc(sizeof(cfr_t));
        if (0 == ctx)
            break;

        ctx->cache = (uint8_t*)audio_malloc(cache_size);
        if (0 == ctx->cache)
            break;

        memcpy(&(ctx->fd), &fd, sizeof(fd));
        ctx->fd_valid = true;
        ctx->cache_size = cache_size;
        ctx->cache_length = 0;
        ctx->cache_offset = 0;

        ctx->super.self = (file_reader_t*)ctx;
        ctx->super.read = read;
        ctx->super.size = size;

#ifdef CFR_MEASURE_CACHE_PERFORMACE
        ctx->cache_hit = 0;
        ctx->cache_miss = 0;
#endif
        return (file_reader_t*)ctx;

    } while (0);

    if (ctx && ctx->cache)
        audio_free(ctx->cache);
    if (ctx)
        audio_free(ctx);
    if (fd_valid)
        f_close(&fd);
    return 0;
}


void cfreader_destroy(file_reader_t *cfr)
{
    cfr_t* ctx = (cfr_t*)cfr;
    if (0 == ctx)
        return;
    if (ctx->cache)
        audio_free(ctx->cache);
    if (ctx->fd_valid)
        f_close(&(ctx->fd));
    audio_free(ctx);
}


#ifdef CFR_MEASURE_CACHE_PERFORMACE

void cfreader_show_cache_status(file_reader_t* cfr)
{
    cfr_t *ctx = (cfr_t *)cfr;
    AUD_LOGI("Audio: cache status: (%llu/%llu), hit %.1f%%\n", ctx->cache_hit, ctx->cache_hit + ctx->cache_miss, (ctx->cache_hit * 100.0f) / (ctx->cache_hit + ctx->cache_miss));
}

#endif

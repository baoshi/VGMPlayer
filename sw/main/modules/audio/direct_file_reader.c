#include <string.h>
#include <ff.h>
#include "audio.h"
#include "direct_file_reader.h"


// Direct File Reader
typedef struct dfr_s
{
    // super class
    file_reader_t super;
    // private fields
    FIL fd;
    bool fd_valid;
} dfr_t;



static size_t read(file_reader_t *self, uint8_t *out, size_t offset, size_t length)
{
    dfr_t *ctx = (dfr_t*)self;
    MY_ASSERT(ctx != NULL && ctx->fd_valid);
    size_t read = 0;
    if (length > 0) 
    {
        if ((offset != (size_t)-1) && (offset != f_tell(&(ctx->fd))))
        {
            if (f_lseek(&(ctx->fd), offset) != FR_OK)
                return 0;
        }
        if (f_read(&(ctx->fd), (void *)out, length, &read) != FR_OK)
        {
                return 0;
        }
        return read;
    }
    return 0;
}


static size_t size(file_reader_t *self)
{
    dfr_t *ctx = (dfr_t*)self;
    return (size_t)f_size(&(ctx->fd));
}


file_reader_t * dfreader_create(const char* fn)
{
    FRESULT fr;
    FIL fd;
    bool fd_valid = false;
    dfr_t *ctx = 0;
    
    do
    {
        fr = f_open(&fd, fn, FA_READ | FA_OPEN_EXISTING);
        if (fr != FR_OK)
            break;
        fd_valid = true;

        ctx = (dfr_t*)audio_malloc(sizeof(dfr_t));
        if (0 == ctx)
            break;
        
        memcpy(&(ctx->fd), &fd, sizeof(fd));
        ctx->fd_valid = true;
        ctx->super.self = (file_reader_t*)ctx;
        ctx->super.read = read;
        ctx->super.size = size;

        return (file_reader_t*)ctx;

    } while (0);

    if (ctx)
        audio_free(ctx);
    if (fd_valid)
        f_close(&fd);
    return NULL;
}


void dfreader_destroy(file_reader_t *dfr)
{
    dfr_t* ctx = (dfr_t*)dfr;
    if (0 == ctx)
        return;
    if (ctx->fd_valid)
        f_close(&(ctx->fd));
    audio_free(ctx);
}

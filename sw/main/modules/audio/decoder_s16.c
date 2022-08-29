#include <string.h>
#include "my_debug.h"
#include "direct_file_reader.h"
#include "decoder_s16.h"


unsigned int decoder_s16_get_samples(decoder_t *me, int16_t *buf, unsigned int len)
{
    decoder_s16_t *ctx = (decoder_s16_t *)me;
    uint8_t *buf8 = (uint8_t *)buf;
    // For len 16-bit samples, we need to read len * 2 8-bit data
    int read8 = (int)ctx->reader->read(ctx->reader, buf8, (size_t)-1, len + len);
    return (unsigned int)(read8 >> 1);
}


decoder_s16_t * decoder_s16_create(char const *file)
{
    decoder_s16_t *decoder = NULL;
    
    do 
    {
        decoder = audio_malloc(sizeof(decoder_s16_t));
        if (NULL == decoder)
        {
            AUD_LOGW("Audio/s16: out of memory\n");
            break;
        }
        memset(decoder, 0, sizeof(decoder_s16_t));
        decoder->reader = dfreader_create(file);
        if (NULL == decoder->reader)
        {
            AUD_LOGW("Audio/s16: open %s failed\n", file);
            break;
        }
        decoder->super.total_samples = (unsigned long)(decoder->reader->size(decoder->reader) / 2);
        decoder->super.get_sample_s16 = (get_sample_s16_t)decoder_s16_get_samples;
        return decoder;
    } while (0);

    if (decoder)
    {
        if (decoder->reader) dfreader_destroy(decoder->reader);
        audio_free(decoder);
    }
    return NULL;
}


void decoder_s16_destroy(decoder_s16_t* decoder)
{
    if (decoder)
    {
        if (decoder->reader) dfreader_destroy(decoder->reader);
        audio_free(decoder);
    }
}

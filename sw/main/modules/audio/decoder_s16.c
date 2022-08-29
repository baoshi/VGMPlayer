#include <string.h>
#include "my_debug.h"
#include "direct_file_reader.h"
#include "decoder_s16.h"


uint32_t decoder_s16_get_samples(decoder_s16_t *me, uint32_t *buf, uint32_t len)
{
    uint16_t *buf16 = (uint16_t *)buf;
    uint8_t *buf8 = (uint8_t *)buf;
    // For len uint32_t samples, we need to read len uint16_t data, or len * 2 uint8_t data
    int read8 = (int)me->reader->read(me->reader, buf8, (size_t)-1, len + len);
    int read16 = (int)(read8 >> 1);
    // extend u16 to u32
    // u16: |AA|BB|CC|DD|EE|  (len: read16 (5))
    // u16: |AA|AA|BB|BB|CC|CC|DD|DD|EE|EE|  (len: read8 (10))
    for (int i = read16 - 1; i >= 0; --i)
    {
        buf16[i + i + 1] = buf16[i];
        buf16[i + i] = buf16[i];
    }
    return (uint32_t)read16;
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
        decoder->super.get_samples = (get_samples_t)decoder_s16_get_samples;
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

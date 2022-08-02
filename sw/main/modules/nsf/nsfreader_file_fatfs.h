#pragma once

#include "nsfreader.h"

#ifdef __cplusplus
extern "C" {
#endif

#define NFR_MEASURE_CACHE_PERFORMACE

nsfreader_ctx_t * nfr_create(const char *fn, uint32_t cache_size);

void nfr_destroy(nsfreader_ctx_t *nfr);

#ifdef NFR_MEASURE_CACHE_PERFORMACE
void nfr_show_cache_status(nsfreader_ctx_t *nfr);
#else
#define nfr_show_cache_status(x) void(x)
#endif


#ifdef __cplusplus
}
#endif
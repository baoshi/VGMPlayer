#ifndef MY_DEBUG_H
#define MY_DEBUG_H

#include <stdint.h>

#define DBG_LEVEL_DBG   0x01
#define DBG_LEVEL_WARN  0x02
#define DBG_LEVEL_ERR   0x03
#define DBG_LEVEL_INFO  0x04
#define DBG_LEVEL_OFF   0x0F


#ifdef __cplusplus
extern "C" {
#endif

extern uint8_t debug_level;

void my_debugf(const char *format, ...) __attribute__((format(printf, 1, 2)));
void my_assert(const char *file, int line, const char *func, const char *pred);

#ifdef __cplusplus
}
#endif

#if defined(DEBUG) || !defined(NDEBUG)
#  define MY_LOGD(fmt, ...) do { if (DBG_LEVEL_DBG  >= debug_level) my_debugf(fmt, ##__VA_ARGS__); } while (0)
#  define MY_LOGW(fmt, ...) do { if (DBG_LEVEL_WARN >= debug_level) my_debugf(fmt, ##__VA_ARGS__); } while (0)
#  define MY_LOGE(fmt, ...) do { if (DBG_LEVEL_ERR  >= debug_level) my_debugf(fmt, ##__VA_ARGS__); } while (0)
#  define MY_LOGI(fmt, ...) do { if (DBG_LEVEL_INFO >= debug_level) my_debugf(fmt, ##__VA_ARGS__); } while (0)
#  define MY_DEBUGF(level, fmt, ...) do { if (level >= debug_level) my_debugf(fmt, ##__VA_ARGS__); } while (0)
#  define MY_ASSERT(__e) ((__e) ? (void)0 : my_assert(__FILE__, __LINE__, __func__, #__e)) 
#else
#  define MY_LOGD(fmt, ...)
#  define MY_LOGW(fmt, ...)
#  define MY_LOGE(fmt, ...)
#  define MY_LOGI(fmt, ...)
#  define MYDEBUGF(level, fmt, message)
#  define MY_ASSERT(__e)
#endif

#endif  // MY_DEBUG_H
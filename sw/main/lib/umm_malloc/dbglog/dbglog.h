#pragma once

#include "my_debug.h"

#define DBGLOG_LEVEL_TRACE (6)
#define DBGLOG_LEVEL_DEBUG (5)
#define DBGLOG_LEVEL_CRITICAL (4)
#define DBGLOG_LEVEL_ERROR (3)
#define DBGLOG_LEVEL_WARNING (2)
#define DBGLOG_LEVEL_INFO (1)
#define DBGLOG_LEVEL_FORCE (0)


#undef DBGLOG_LEVEL
#define DBGLOG_LEVEL DBGLOG_LEVEL_DEBUG

#define DBGLOG_32_BIT_PTR(x) ((uint32_t)(((uintptr_t)(x)) & 0xffffffff))

#undef DBGLOG_TRACE
#undef DBGLOG_DEBUG
#undef DBGLOG_CRITICAL
#undef DBGLOG_ERROR
#undef DBGLOG_WARNING
#undef DBGLOG_INFO
#undef DBGLOG_FORCE

#if DBGLOG_LEVEL >= DBGLOG_LEVEL_TRACE
#define DBGLOG_TRACE(format, ...) do { my_debugf(format, ##__VA_ARGS__); } while (0)
#else
#define DBGLOG_TRACE(format, ...)
#endif

#if DBGLOG_LEVEL >= 5
#define DBGLOG_DEBUG(format, ...) do { my_debugf(format, ##__VA_ARGS__); } while (0)
#else
#define DBGLOG_DEBUG(format, ...)
#endif

#if DBGLOG_LEVEL >= 4
#define DBGLOG_CRITICAL(format, ...) do { my_debugf(format, ##__VA_ARGS__); } while (0)
#else
#define DBGLOG_CRITICAL(format, ...)
#endif

#if DBGLOG_LEVEL >= 3
#define DBGLOG_ERROR(format, ...) do { my_debugf(format, ##__VA_ARGS__); } while (0)
#else
#define DBGLOG_ERROR(format, ...)
#endif

#if DBGLOG_LEVEL >= 2
#define DBGLOG_WARNING(format, ...) do { my_debugf(format, ##__VA_ARGS__); } while (0)
#else
#define DBGLOG_WARNING(format, ...)
#endif

#if DBGLOG_LEVEL >= 1
#define DBGLOG_INFO(format, ...) do { my_debugf(format, ##__VA_ARGS__); } while (0)
#else
#define DBGLOG_INFO(format, ...)
#endif

#define DBGLOG_FORCE(force, format, ...) { if (force) { my_debugf(format, ##__VA_ARGS__); } }



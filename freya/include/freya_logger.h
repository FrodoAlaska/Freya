#pragma once

#include "freya_base.h"
#include "freya_containers.h"

//////////////////////////////////////////////////////////////////////////

namespace freya { // Start of freya

///---------------------------------------------------------------------------------------------------------------------
/// DEFS
 
#define FREYA_LOG_INFO_ACTIVE 1 
#define FREYA_LOG_WARN_ACTIVE 1 

// Only activate trace and debug logs on debug builds

#if FREYA_BUILD_RELEASE == 1
#define FREYA_LOG_TRACE_ACTIVE 0
#define FREYA_LOG_DEBUG_ACTIVE 0
#else 
#define FREYA_LOG_TRACE_ACTIVE 1
#define FREYA_LOG_DEBUG_ACTIVE 1
#endif

/// DEFS
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Log level
enum LogLevel {
  LOG_LEVEL_TRACE,
  LOG_LEVEL_DEBUG, 
  LOG_LEVEL_INFO,
  LOG_LEVEL_WARN,
  LOG_LEVEL_ERROR, 
  LOG_LEVEL_FATAL,
};
/// Log level
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Logger functions

/// Log an assertion with the given information.
FREYA_API void logger_log_assert(const char* expr, const char* msg, const char* file, const u32 line_num);

/// Log a specific log level `lvl` with the given `msg` and any other parametars.
FREYA_API void logger_log(const LogLevel lvl, const char* msg, ...);

/// Logger functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Trace log

#if FREYA_LOG_TRACE_ACTIVE == 1
#define FREYA_LOG_TRACE(msg, ...) freya::logger_log(freya::LOG_LEVEL_TRACE, msg, ##__VA_ARGS__)
#else
#define FREYA_LOG_TRACE(msg, ...)
#endif

/// Trace log
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Debug log

#if FREYA_LOG_DEBUG_ACTIVE == 1
#define FREYA_LOG_DEBUG(msg, ...) freya::logger_log(freya::LOG_LEVEL_DEBUG, msg, ##__VA_ARGS__)
#else
#define FREYA_LOG_DEBUG(msg, ...)
#endif

/// Debug log
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Info log

#if FREYA_LOG_INFO_ACTIVE == 1
#define FREYA_LOG_INFO(msg, ...) freya::logger_log(freya::LOG_LEVEL_INFO, msg, ##__VA_ARGS__)
#else
#define FREYA_LOG_INFO(msg, ...)
#endif

/// Info log
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Warn log

#if FREYA_LOG_WARN_ACTIVE == 1
#define FREYA_LOG_WARN(msg, ...) freya::logger_log(freya::LOG_LEVEL_WARN, msg, ##__VA_ARGS__)
#else
#define FREYA_LOG_WARN(msg, ...)
#endif

/// Warn log
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Error log

#define FREYA_LOG_ERROR(msg, ...) freya::logger_log(freya::LOG_LEVEL_ERROR, msg, ##__VA_ARGS__)

/// Error log
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Fatal log

#define FREYA_LOG_FATAL(msg, ...) freya::logger_log(freya::LOG_LEVEL_FATAL, msg, ##__VA_ARGS__)

/// Fatal log
///---------------------------------------------------------------------------------------------------------------------

/// ---------------------------------------------------------------------
/// Asserts

// Only enable asserts outside of distribution builds

#ifndef FREYA_BUILD_DISTRIBUTION
  #define FREYA_ASSERTS_ENABLED 
#endif

#ifdef FREYA_ASSERTS_ENABLED

/// Windows debug break

#if FREYA_PLATFORM_WINDOWS == 1
  #define DEBUG_BREAK() __debugbreak()

/// Linux debug break

#elif FREYA_PLATFORM_LINUX == 1
  #define DEBUG_BREAK() __builtin_trap()

#endif

/// Assert if `expr` is false.

#define FREYA_ASSERT(expr)                                            \
        {                                                             \
          if(expr) {                                                  \
          }                                                           \
          else {                                                      \
            freya::logger_log_assert(#expr, "", __FILE__, __LINE__);  \
            DEBUG_BREAK();                                            \
          }                                                           \
        }


/// Assert if `expr` is false, using `msg` to log the information.

#define FREYA_ASSERT_LOG(expr, msg)                                   \
        {                                                             \
          if(expr) {                                                  \
          }                                                           \
          else {                                                      \
            freya::logger_log_assert(#expr, msg, __FILE__, __LINE__); \
            DEBUG_BREAK();                                            \
          }                                                           \
        }

/// Same as the regular asserts, but these only work in debug builds.

#if FREYA_BUILD_DEBUG == 1
#define FREYA_DEBUG_ASSERT(expr, msg)                                 \
        {                                                             \
          if(expr) {                                                  \
          }                                                           \
          else {                                                      \
            freya::logger_log_assert(#expr, msg, __FILE__, __LINE__); \
            DEBUG_BREAK();                                            \
          }                                                           \
        }
#endif

#else 
  #define FREYA_ASSERT(expr, msg)
#endif

/// Asserts
/// ---------------------------------------------------------------------

} // End of freya

//////////////////////////////////////////////////////////////////////////

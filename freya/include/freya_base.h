#pragma once

/// ----------------------------------------------------------------------
/// Platform detection

/// Windows

#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
  #define FREYA_PLATFORM_WINDOWS 1

/// Web

#elif defined(__EMSCRIPTEN__)
  #define FREYA_PLATFORM_WEB    1 

/// Linux

#elif defined(__linux__) || defined(__gnu_linux__)
  #define FREYA_PLATFORM_LINUX  1 
#endif

/// Platform detection
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Build Types

#if defined(DEBUG) || defined(_DEBUG)
  #define FREYA_BUILD_DEBUG   1 
  #define FREYA_BUILD_RELEASE 0
#else
  #define FREYA_BUILD_DEBUG   0 
  #define FREYA_BUILD_RELEASE 1
#endif

/// Build Types
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// DEFS

/// Imports

// Windows

#if FREYA_PLATFORM_WINDOWS == 1
  #define FREYA_API __declspec(dllexport) 

// Linux

#elif FREYA_PLATFORM_LINUX == 1
  #define FREYA_API __attribute__((visibility("default"))) 

// Web

#elif FREYA_PLATFORM_WEB == 1
  #include <emscripten.h> // @TODO (Web): Maybe don't do this here?
  #define FREYA_API EMSCRIPTEN_KEEPALIVE
#endif

/// Debug break

// Windows

#if FREYA_PLATFORM_WINDOWS == 1
  #define FREYA_DEBUG_BREAK() __debugbreak()

// Linux

#elif FREYA_PLATFORM_LINUX == 1
  #define FREYA_DEBUG_BREAK() __builtin_trap()

// Web

#elif FREYA_PLATFORM_WEB == 1
  #define FREYA_DEBUG_BREAK() __builtin_trap() 
#endif

/// DEFS
/// ----------------------------------------------------------------------

/// @NOTE: 
///
/// Keeping these includes here to not conflict with any platform-specifc 
/// includes above it.
///

#include <cstddef>
#include <cstdint>

//////////////////////////////////////////////////////////////////////////

namespace freya { // Start of freya
 
/// ----------------------------------------------------------------------
/// Typedefs

/// char
typedef char          i8; 

/// short
typedef short         i16;

/// int
typedef int           i32;

/// long
typedef long          i64;

/// unsigned char
typedef std::uint8_t  u8;

/// unsigned short
typedef std::uint16_t u16;   

/// unsigned int
typedef std::uint32_t u32;   

/// unsigned long
typedef std::uint64_t u64;   

/// size_t
typedef size_t        sizei;

/// uintptr_t 
typedef uintptr_t     uintptr;

/// float
typedef float         f32;

/// double
typedef double        f64;

/// Typedefs
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Macros

/// Returns `true` if `other` is set in `bit`
#define IS_BIT_SET(bit, other) ((bit & other) == other)

/// Sets/adds `other` into `bit`
#define SET_BIT(bit, other)    (bit |= other)

/// Unsets/removes `other` from `bit`
#define UNSET_BIT(bit, other)  (bit &= ~(other))

/// Macros
/// ----------------------------------------------------------------------

} // End of freya

//////////////////////////////////////////////////////////////////////////

#pragma once

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
/// DEFS

/// Exports

#ifdef FREYA_EXPORT 

  /// Window exports
  
  #ifdef _MSC_VER
    #define FREYA_API __declspec(dllexport) 
  
  /// Linux exports
 
  #else
    #define FREYA_API __attribute__((visibility("default"))) 
  #endif

#endif

/// Window imports

#ifdef _MSC_VER 
  #define FREYA_API __declspec(dllexport) 

/// Linux exports

#else
  #define FREYA_API __attribute__((visibility("default"))) 
#endif

/// DEFS
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

} // End of freya

//////////////////////////////////////////////////////////////////////////

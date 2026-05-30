#pragma once

#include "freya_window.h"
#include "freya_memory.h"

#include "sokol/sokol_gfx.h"

//////////////////////////////////////////////////////////////////////////

namespace freya { // Start of freya

///---------------------------------------------------------------------------------------------------------------------
/// DEFS

/// Use OpenGL on Windows and Linux builds

#if (FREYA_PLATFORM_WINDOWS == 1 || FREYA_PLATFORM_LINUX == 1) 
  #define FREYA_GFX_GL   1

/// Use OpenGL-ES on Web builds

#elif (FREYA_PLATFORM_WEB == 1)
  #define FREYA_GFX_GLES 1

/// Invalid platform 

#else 
  #error "[FREYA-FATAL]: Unsupported graphics API detected!"
#endif

/// Only supported OpenGL version

#define FREYA_GL_VERSION 4
#define FREYA_GL_VERSION 3

/// Only supported OpenGL-ES version

#define FREYA_GLES_VERSION 3
#define FREYA_GLES_VERSION 0

/// DEFS
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Color
using Color = Vec4;
/// Color
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Color consts

/// Predefined colors for ease-of-use.

const Color COLOR_WHITE  = Color(1.0f);
const Color COLOR_BLACK  = Color(0.0f, 0.0f, 0.0f, 1.0f);
const Color COLOR_RED    = Color(1.0f, 0.0f, 0.0f, 1.0f);
const Color COLOR_GREEN  = Color(0.0f, 1.0f, 0.0f, 1.0f);
const Color COLOR_BLUE   = Color(0.0f, 0.0f, 1.0f, 1.0f);
const Color COLOR_YELLOW = Color(1.0f, 1.0f, 0.0f, 1.0f);
const Color COLOR_CYAN   = Color(0.0f, 1.0f, 1.0f, 1.0f);
const Color COLOR_PURPLE = Color(1.0f, 0.0f, 1.0f, 1.0f);

/// Color consts
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Texture
struct Texture {
  
  sg_image image;
  sg_sampler sampler;
  sg_view view;

  IVec2 size = IVec2(0);
};
/// Texture
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Font
struct Font {
  /// @NOTE:
  ///
  /// The library we currently use for UI (RmlUi) uses FreeType to load 
  /// the fonts and deal with all that logic. All it needs from us, however, 
  /// is the byte data loaded from a truetype font. And `font_data` is just that. 
  ///
  /// This is clearly not a good approach. But, in the future, we will make RmlUi 
  /// rely on _our_ font data. For now, though, this is how it works.
  ///

  String name;
  DynamicArray<u8> font_data;
};
/// Font
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Color functions

/// Return a new RGB `Color` from the given `hex_color`. 
///
/// @NOTE: The given hex color should be arranged as follows: `0xRRGGBBAA`.
FREYA_API Color color_hex_to_rgb(const u32 hex_color);

/// Return a new HEX value from the given `rgb` color.
///
/// @NOTE: The returned hex color will be arranged as follows: `0xRRGGBBAA`.
FREYA_API u32 color_rgb_to_hex(const Color& rgb);

/// Lerp the given `color` towards `other` by `delta`.
FREYA_API void color_lerp(Color& color, const Color& other, const f32 delta);

/// Use smoothstep to interpolate the given `color` towards `other` by `amount`.
FREYA_API void color_slerp(Color& color, const Color& other, const Color& amount);

/// Color functions
///---------------------------------------------------------------------------------------------------------------------

} // End of freya

//////////////////////////////////////////////////////////////////////////

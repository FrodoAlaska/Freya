#include "freya_render.h"

//////////////////////////////////////////////////////////////////////////

namespace freya { // Start of freya

///---------------------------------------------------------------------------------------------------------------------
/// Color functions

Color color_hex_to_rgb(const u32 hex_color) {
  Color color;

  color.r = (hex_color & 0xFF000000) / 255.0f;
  color.g = (hex_color & 0x00FF0000) / 255.0f;
  color.b = (hex_color & 0x0000FF00) / 255.0f;
  color.a = (hex_color & 0x000000FF) / 255.0f;

  return color;
}

u32 color_rgb_to_hex(const Color& rgb) {
  IVec4 irgb = (IVec4(rgb) * 255) / 16; // Converting to hex

  return ((irgb.r & 0xFF) << 24) + 
         ((irgb.g & 0xFF) << 16) +
         ((irgb.b & 0xFF) << 8)  +
         ((irgb.a & 0xFF)); // @TODO (Color): The alpha channel is fucked
}

void color_lerp(Color& color, const Color& other, const f32 delta) {
  color = vec4_lerp(color, other, delta);
}

void color_slerp(Color& color, const Color& other, const Color& amount) {
  color = vec4_smoothstep(color, other, amount);
}

/// Color functions
///---------------------------------------------------------------------------------------------------------------------

} // End of freya

//////////////////////////////////////////////////////////////////////////

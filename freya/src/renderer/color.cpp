#include "freya_gfx.h"
#include "freya_logger.h"

//////////////////////////////////////////////////////////////////////////

namespace freya { // Start of freya

///---------------------------------------------------------------------------------------------------------------------
/// Color functions

Color color_hex_to_rgb(u32 hex_color) {
  Color color;

  color.r = (hex_color & 0xFF000000) / 255.0f;
  color.g = (hex_color & 0x00FF0000) / 255.0f;
  color.b = (hex_color & 0x0000FF00) / 255.0f;
  color.a = (hex_color & 0x000000FF) / 255.0f;

  return color;
}

void color_lerp(Color& color, const Color& other, f32 delta) {
  color = vec4_lerp(color, other, delta);
}

void color_slerp(Color& color, const Color& other, const Color& amount) {
  color = vec4_smoothstep(color, other, amount);
}

Clay_Color color_convert_clay(const Color& color) {
  return Clay_Color{
    .r = (color.r * 255.0f), 
    .g = (color.g * 255.0f), 
    .b = (color.b * 255.0f), 
    .a = (color.a * 255.0f), 
  };
}

/// Color functions
///---------------------------------------------------------------------------------------------------------------------

} // End of freya

//////////////////////////////////////////////////////////////////////////

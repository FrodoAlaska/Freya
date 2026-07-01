#include "freya_ui.h"
#include "freya_render.h"

#include "fontstash/fontstash.h"

//////////////////////////////////////////////////////////////////////////

namespace freya { // Start of freya

///---------------------------------------------------------------------------------------------------------------------
/// Private functions

static void measure_bounds(UIText& text, const Vec2& padding) {
  FONScontext* fons = (FONScontext*)renderer_get_font_context();

  // Get the horizontal bounds
  
  f32 bounds;
  fonsTextBounds(fons, padding.x, padding.y, text.string.c_str(), nullptr, &bounds);

  // Get the vertical bounds

  f32 min, max;
  fonsLineBounds(fons, padding.y, &min, &max);

  // Set the text's bounds

  text.bounds.x = bounds;
  text.bounds.y = max;
}

/// Private functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// UIText functions

void ui_text_create(UIText& out_text, const UITextDesc& desc) {
  out_text.anchor = desc.anchor;
  out_text.align  = (FONS_ALIGN_LEFT | FONS_ALIGN_BASELINE);

  out_text.string = desc.string;
  out_text.font   = asset_group_get_font(desc.font_id);

  out_text.size    = desc.font_size;
  out_text.blur    = desc.blur_effect;
  out_text.spacing = desc.spacing;

  out_text.offset = desc.offset;
  out_text.color  = desc.color;

  out_text.canvas_bounds = desc.canvas_bounds;

  out_text.is_active = true;
  ui_text_place(out_text);
}

void ui_text_place(UIText& text) {
  Vec2 bounds        = text.canvas_bounds;
  Vec2 bounds_center = text.canvas_bounds / 2.0f;
  
  Vec2 text_center = text.bounds / 2.0f;
  Vec2 padding     = Vec2(10.0f);

  switch(text.anchor) {
    case UI_ANCHOR_TOP_LEFT:  
      text.position = padding;
      text.align    = (FONS_ALIGN_LEFT | FONS_ALIGN_BASELINE);
      break;
    case UI_ANCHOR_TOP_CENTER:
      text.position.x = bounds_center.x; 
      text.position.y = padding.y; 
      
      text.align = (FONS_ALIGN_CENTER | FONS_ALIGN_BASELINE);
      break;
    case UI_ANCHOR_TOP_RIGHT:
      text.position.x = bounds.x - padding.x; 
      text.position.y = padding.y;  
      
      text.align = (FONS_ALIGN_RIGHT | FONS_ALIGN_BASELINE);
      break;
    case UI_ANCHOR_CENTER_LEFT:  
      text.position.x = padding.x;
      text.position.y = bounds_center.y; 
      
      text.align = (FONS_ALIGN_LEFT | FONS_ALIGN_BASELINE);
      break;
    case UI_ANCHOR_CENTER:
      text.position = bounds_center;
      text.align    = (FONS_ALIGN_CENTER | FONS_ALIGN_BASELINE);
      break;
    case UI_ANCHOR_CENTER_RIGHT:
      text.position.x = bounds.x - padding.x; 
      text.position.y = bounds_center.y; 
      
      text.align = (FONS_ALIGN_RIGHT | FONS_ALIGN_BASELINE);
      break;
    case UI_ANCHOR_BOTTOM_LEFT:  
      text.position.x = padding.x;
      text.position.y = bounds.y - padding.y; 
      
      text.align = (FONS_ALIGN_LEFT | FONS_ALIGN_BOTTOM);
      break;
    case UI_ANCHOR_BOTTOM_CENTER:
      text.position.x = bounds_center.x;
      text.position.y = bounds.y - padding.y; 
      
      text.align = (FONS_ALIGN_CENTER | FONS_ALIGN_BOTTOM);
      break;
    case UI_ANCHOR_BOTTOM_RIGHT:
      text.position = bounds - padding; 
      text.align    = (FONS_ALIGN_RIGHT | FONS_ALIGN_BOTTOM);
      break;
  }

  // Place the text correctly
  
  measure_bounds(text, padding);
  text.position.y += text.bounds.y; 
  text.position   += text.offset; 
}

/// UIText functions
///---------------------------------------------------------------------------------------------------------------------

} // End of freya

//////////////////////////////////////////////////////////////////////////

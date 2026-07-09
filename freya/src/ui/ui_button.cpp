#include "freya_ui.h"
#include "freya_render.h"
#include "freya_input.h"

//////////////////////////////////////////////////////////////////////////

namespace freya { // Start of freya

///---------------------------------------------------------------------------------------------------------------------
/// UIButton functions

void ui_button_create(UIButton& out_button, const UIButtonDesc& desc) {
  // Init the button

  out_button.anchor  = desc.anchor; 

  out_button.offset        = desc.offset; 
  out_button.padding       = desc.padding;
  out_button.canvas_bounds = desc.canvas_bounds;

  out_button.size = Vec3(desc.size.x, desc.size.y, desc.outline_thickness);

  out_button.color         = desc.color;
  out_button.outline_color = desc.outline_color;

  out_button.layer     = desc.layer;
  out_button.is_active = true;

  out_button.texture = (desc.texture_id.get_id() != ASSET_ID_INVALID) ? asset_group_get_texture(desc.texture_id) : Texture{};

  // Init the text 

  UITextDesc text_desc = {
    .string    = desc.string, 
    .font_id   = desc.font_id, 
    .font_size = desc.font_size,

    .anchor        = desc.anchor, 
    .canvas_bounds = desc.canvas_bounds,

    .offset = desc.offset, 
    .color  = desc.text_color, 
    .layer  = desc.layer,
  };
  ui_text_create(out_button.text, text_desc);

  // Done!
  ui_button_place(out_button);
}

void ui_button_place(UIButton& button) {
  // Place the text first

  button.text.anchor  = button.anchor;
  button.text.offset  = button.offset;
  button.text.padding = button.padding;

  ui_text_place(button.text);

  // Adjust the button after the text

  switch(button.anchor) {
    case UI_ANCHOR_TOP_LEFT:  
      break;
    case UI_ANCHOR_TOP_CENTER:
      break;
    case UI_ANCHOR_TOP_RIGHT:
      break;
    case UI_ANCHOR_CENTER_LEFT:  
      break;
    case UI_ANCHOR_CENTER:
      button.position.x = button.text.position.x;
      button.position.y = (button.text.position.y - button.padding.y);
      break;
    case UI_ANCHOR_CENTER_RIGHT:
      break;
    case UI_ANCHOR_BOTTOM_LEFT:  
      break;
    case UI_ANCHOR_BOTTOM_CENTER:
      break;
    case UI_ANCHOR_BOTTOM_RIGHT:
      break;
  }
}

bool ui_button_hovered(UIButton& button) {
  // Set the default state

  bool hovered   = false;
  button.color.a = 1.0f;

  // Check if the mouse is hovering over the button

  IVec2 mouse_pos = input_mouse_position();
  Vec2 button_pos = button.position - (Vec2(button.size) / 2.0f);

  if(point_in_rect(mouse_pos, button_pos, Vec2(button.size))) {
    hovered        = true;
    button.color.a = 0.5f;
  }

  // Done!
  return hovered;
}

bool ui_button_pressed(UIButton& button) {
  return ui_button_hovered(button) && input_action_pressed("ui-click");
}

/// UIButton functions
///---------------------------------------------------------------------------------------------------------------------

} // End of freya

//////////////////////////////////////////////////////////////////////////

#include "freya_ui.h"
#include "freya_render.h"

//////////////////////////////////////////////////////////////////////////

namespace freya { // Start of freya

///---------------------------------------------------------------------------------------------------------------------
/// UISprite functions

void ui_sprite_create(UISprite& out_sprite, const UISpriteDesc& desc) {
  out_sprite.texture = (desc.texture_id.get_id() != ASSET_ID_INVALID) ? asset_group_get_texture(desc.texture_id) : Texture{};
  out_sprite.anchor  = desc.anchor;

  out_sprite.position      = Vec2(0.0f);
  out_sprite.offset        = desc.offset;
  out_sprite.size          = desc.size;
  out_sprite.canvas_bounds = desc.canvas_bounds;

  out_sprite.color = desc.color; 
  out_sprite.layer = desc.layer; 

  out_sprite.is_active = true;
  ui_sprite_place(out_sprite);
}

void ui_sprite_place(UISprite& sprite) {
  Vec2 bounds        = sprite.canvas_bounds;
  Vec2 bounds_center = sprite.canvas_bounds / 2.0f;

  switch(sprite.anchor) {
    case UI_ANCHOR_TOP_LEFT: 
      sprite.position = (sprite.size / 2.0f);
      break;
    case UI_ANCHOR_TOP_CENTER:
      sprite.position.x = bounds_center.x;
      sprite.position.y = 0.0f;
      break;
    case UI_ANCHOR_TOP_RIGHT:
      sprite.position.x = bounds.x;
      sprite.position.y = 0.0f;
      break;
    case UI_ANCHOR_CENTER_LEFT:  
      break;
    case UI_ANCHOR_CENTER:
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

  sprite.position += sprite.offset;
}

/// UISprite functions
///---------------------------------------------------------------------------------------------------------------------

} // End of freya

//////////////////////////////////////////////////////////////////////////

#include "freya_ui.h"
#include "freya_render.h"

//////////////////////////////////////////////////////////////////////////

namespace freya { // Start of freya

///---------------------------------------------------------------------------------------------------------------------
/// UISprite functions

void ui_sprite_create(UISprite& out_sprite, const UISpriteDesc& desc) {
  out_sprite.texture = (desc.texture_id.get_id() != ASSET_ID_INVALID) ? asset_group_get_texture(desc.texture_id) : Texture{};
  out_sprite.anchor  = desc.anchor;

  out_sprite.position = Vec2(0.0f);
  out_sprite.offset   = desc.offset;
  out_sprite.size     = desc.size;

  out_sprite.padding       = desc.padding;
  out_sprite.canvas_bounds = desc.canvas_bounds;

  out_sprite.color = desc.color; 
  out_sprite.layer = desc.layer; 

  out_sprite.is_active = true;
  ui_sprite_place(out_sprite);
}

void ui_sprite_place(UISprite& sprite) {
  Vec2 bounds        = sprite.canvas_bounds;
  Vec2 bounds_center = sprite.canvas_bounds / 2.0f;

  Vec2 sprite_center = (sprite.size / 2.0f);

  switch(sprite.anchor) {
    case UI_ANCHOR_TOP_LEFT: 
      sprite.position = sprite_center + sprite.padding;
      break;
    case UI_ANCHOR_TOP_CENTER:
      sprite.position.x = bounds_center.x;
      sprite.position.y = sprite_center.y + sprite.padding.y;
      break;
    case UI_ANCHOR_TOP_RIGHT:
      sprite.position.x = bounds.x - sprite_center.x - sprite.padding.x;
      sprite.position.y = sprite_center.y + sprite.padding.y;
      break;
    case UI_ANCHOR_CENTER_LEFT:  
      sprite.position.x = sprite_center.x + sprite.padding.x;
      sprite.position.y = bounds_center.y;
      break;
    case UI_ANCHOR_CENTER:
      sprite.position = bounds_center;
      break;
    case UI_ANCHOR_CENTER_RIGHT:
      sprite.position.x = bounds.x - sprite_center.x - sprite.padding.x;
      sprite.position.y = bounds_center.y;
      break;
    case UI_ANCHOR_BOTTOM_LEFT:  
      sprite.position.x = sprite_center.x + sprite.padding.x;
      sprite.position.y = bounds.y - sprite_center.y - sprite.padding.y;
      break;
    case UI_ANCHOR_BOTTOM_CENTER:
      sprite.position.x = bounds_center.x; 
      sprite.position.y = bounds.y - sprite_center.y - sprite.padding.y;
      break;
    case UI_ANCHOR_BOTTOM_RIGHT:
      sprite.position = bounds - sprite_center - sprite.padding;
      break;
  }

  sprite.position += sprite.offset;
}

/// UISprite functions
///---------------------------------------------------------------------------------------------------------------------

} // End of freya

//////////////////////////////////////////////////////////////////////////

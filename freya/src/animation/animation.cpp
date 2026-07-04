#include "freya_render.h"

//////////////////////////////////////////////////////////////////////////

namespace freya { // Start of freya

///---------------------------------------------------------------------------------------------------------------------
/// Animation functions

void animation_create(Animation& out_anim, const AnimationDesc& desc) {
  // Texture and sizes init

  out_anim.texture    = asset_group_get_texture(desc.texture_id);
  out_anim.frame_size = desc.frame_size; 

  // Frames init

  IVec2 tex_size = out_anim.texture.size;
  
  out_anim.frames_count  = (tex_size.x / out_anim.frame_size.x) - 1;
  out_anim.loops         = 0;

  out_anim.start         = desc.start;
  out_anim.direction     = desc.direction;
  out_anim.current_frame = desc.start;

  // Animation logic init

  out_anim.can_loop      = desc.can_loop;
  out_anim.can_alternate = desc.can_alternate;

  // Timer init
  
  out_anim.counter    = 0.0f;
  out_anim.flip_speed = desc.flip_speed; 
  out_anim.is_active  = true;

  // Source rect init

  out_anim.src_rect = Rect2D {
    .size     = out_anim.frame_size, 
    .position = Vec2(0.0f), 
  };
}

void animation_update(Animation& anim, const f32 delta_time) {
  // Invactive animation... don't do anything

  if(!anim.is_active) {
    return;
  } 

  // Manage the timers

  anim.counter += delta_time; 
  if(anim.counter >= anim.flip_speed) {
    anim.counter        = 0.0f;
    anim.current_frame += anim.direction;
  }

  // Calculate the source rectangle to be used by the renderer

  anim.src_rect = Rect2D {
    .size     = anim.frame_size,
    .position = (Vec2)(anim.current_frame) * anim.frame_size, 
  };

  // The animation is not done yet... defer the 
  // other logic for when it is 

  bool x_done = (anim.current_frame.x >= 0 && anim.current_frame.x < anim.frames_count);
  bool y_done = (anim.current_frame.y >= 0 && anim.current_frame.y < anim.frames_count);

  if(x_done && y_done) {
    return;
  }

  // Manage the state after an animation ends

  if(!anim.can_loop && !anim.can_alternate) {
    anim.is_active = false;
    return;
  }
  else if(anim.can_loop) {
    anim.current_frame = anim.start;
  }
  else if(anim.can_alternate) {
    anim.direction *= -1;
  }
  
  anim.loops++;

  // Clamp the animation frames for safety
  
  anim.current_frame.x = clamp_int(anim.current_frame.x, 0, anim.frames_count);
  anim.current_frame.y = clamp_int(anim.current_frame.y, 0, anim.frames_count);
}

void animation_reset(Animation& anim) {
  anim.current_frame = anim.start;
  anim.loops         = 0;
  anim.counter       = 0.0f;
  anim.is_active     = true;

  anim.src_rect = Rect2D {
    .size     = anim.frame_size,
    .position = Vec2(0.0f),
  };
}

/// Animation functions
///---------------------------------------------------------------------------------------------------------------------

} // End of freya

//////////////////////////////////////////////////////////////////////////

#pragma once

#include "freya_assets.h"
#include "freya_math.h"
#include "freya_timer.h"

//////////////////////////////////////////////////////////////////////////

namespace freya { // Start of freya

///---------------------------------------------------------------------------------------------------------------------
/// AnimationDesc
struct AnimationDesc {
  /// The ID of the animation texture to 
  /// be used for this animation.
  AssetID texture_id; 
 
  /// The size of each indivisual frame 
  /// in the animation.
  Vec2 frame_size;

  /// The speed of the animation. 
  f32 flip_speed;

  /// The index of the starting row 
  /// of the animation. 
  ///
  /// This can be used if a texture 
  /// includes multiple animations. 
  ///
  /// @NOTE: The default values is `0`.
  i32 start_row      = 0;

  /// Indicates whether the animation 
  /// can loop to the beginning when finished.
  ///
  /// @NOTE: The default value is `true`.
  bool can_loop      = true;

  /// Indicates whether the animation can 
  /// alternate directions when finished.
  ///
  /// @NOTE: The default value is `true`.
  bool can_alternate = false;

  /// Indicates whether to play the animation 
  /// in reverse order
  ///
  /// @NOTE: The default value is `false`.
  bool is_reversed   = false;
};
/// AnimationDesc
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Animation
struct Animation {
  Texture texture; 
  Vec2 frame_size;

  i32 current_frame, frames_count;
  i32 direction, start_row, loops;

  f32 counter, flip_speed;
  bool is_active, can_loop, can_alternate;

  Rect2D src_rect;
};
/// Animation
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Animator
struct Animator {
  DynamicArray<Animation> animations;
  HashMap<String, i32> remaps;

  i32 current_animation = 0; 
  i32 next_animation    = 0;

  f32 speed = 0.1f;

  bool is_immediate = true;
  bool is_playing   = true; 

  bool is_switching = false;
};
/// Animator
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Animation functions

/// Initialize an animation `out_anim` using the information found in `desc`.
FREYA_API void animation_create(Animation& out_anim, const AnimationDesc& desc);

/// Update the given `anim` scaled to `delta_time`.
FREYA_API void animation_update(Animation& anim, const f32 delta_time);

/// Reset the given `anim` to its initial state.
FREYA_API void animation_reset(Animation& anim);

/// Animation functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Animator functions

/// Create an animation using the given `anim_id` and `frame_size`, and push it into `aniamtor` identified by `name`.
FREYA_API void animator_push_animation(Animator& animator, const String& name, const Vec2 frame_size, const AssetID& anim_id);

/// Switch the current animation of `animator` to the animation identified by `name` or `anim_index`.
///
/// @NOTE: If the `is_immediate` flag is turned off, the animator will not switch 
/// until the current animation is done playing. Otherwise, the switch is immediate.
FREYA_API void animator_switch(Animator& animator, const i32 anim_index);
FREYA_API void animator_switch(Animator& animator, const String& name);

/// Update the given `animator` scaled to `delta_time`.
FREYA_API void animator_update(Animator& animator, const f32 delta_time);

/// Reset the given `animator` to its initial state.
FREYA_API void animator_reset(Animator& animator);

/// Clear the given `animator` completely, removing any aniamtions.
FREYA_API void animator_clear(Animator& animator);

/// Animator functions
///---------------------------------------------------------------------------------------------------------------------

} // End of freya

//////////////////////////////////////////////////////////////////////////

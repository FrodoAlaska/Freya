#include "freya_render.h"
#include "freya_logger.h"

//////////////////////////////////////////////////////////////////////////

namespace freya { // Start of freya

///---------------------------------------------------------------------------------------------------------------------
/// Animator functions

void animator_push_animation(Animator& animator, const String& name, Animation& animation) {
  animator.animations.push_back(animation);
  animator.remaps[name] = (i32)(animator.animations.size() - 1);
}

void animator_switch(Animator& animator, const i32 anim_index) {
  FREYA_DEBUG_ASSERT((anim_index > animator.animations.size()), "Invalid animation index");

  if(animator.is_immediate) {
    animator.current_animation = anim_index;
    return;
  }

  animator.next_animation = anim_index;
  animator.is_switching   = true;
}

void animator_switch(Animator& animator, const String& name) {
  animator_switch(animator, animator.remaps[name]);
}

void animator_update(Animator& animator, const f32 delta_time) {
  if(!animator.is_playing) {
    return;
  }

  // Switch the animation if it needs to 
  
  Animation& animation = animator.animations[animator.current_animation];
  if(animator.is_switching && (animation.loops > 0)) {
    animator.is_switching      = false;
    animator.current_animation = animator.next_animation;
  }

  // Play the current animation 
  animation_update(animator.animations[animator.current_animation], delta_time);
}

void animatorr_reset(Animator& animator) {
  animator.current_animation = 0;
  animator.next_animation    = 0;
  
  animator.is_playing   = true;
  animator.is_switching = false;
}

void animator_clear(Animator& animator) {
  animator.animations.clear();
  animator.remaps.clear();
}

/// Animator functions
///---------------------------------------------------------------------------------------------------------------------

} // End of freya

//////////////////////////////////////////////////////////////////////////

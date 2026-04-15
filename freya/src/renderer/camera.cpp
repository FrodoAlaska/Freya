#include "freya_render.h"
#include "freya_input.h"
#include "freya_logger.h"

//////////////////////////////////////////////////////////////////////////

namespace freya { // Start of freya

/// ----------------------------------------------------------------------
/// Camera functions

void camera_create(Camera& out_camera, const CameraDesc& desc) {
  FREYA_DEBUG_ASSERT((out_camera.view_bounds.x >= 0 && out_camera.view_bounds.y >= 0), "Invalid camera view bounds!");

  out_camera.position    = desc.position;
  out_camera.view_bounds = desc.view_bounds;

  out_camera.rotation = desc.rotation;
  out_camera.zoom     = desc.zoom;
  out_camera.exposure = desc.exposure;
}

void camera_move_top_down(Camera& cam, const Vec2& speed, const f32 delta_time) {
  Vec2 direction = Vec2(0.0f);

  if(input_key_down(KEY_W)) {
    direction.y = -1.0f;
  }
  else if(input_key_down(KEY_S)) {
    direction.y = 1.0f;
  }

  if(input_key_down(KEY_A)) {
    direction.x = -1.0f;
  }
  else if(input_key_down(KEY_D)) {
    direction.x = 1.0f;
  }

  cam.position += direction * speed * delta_time;
}

void camera_move_side_scroller(Camera& cam, const f32 speed, const f32 delta_time) {
  f32 direction = 0.0f;

  if(input_key_down(KEY_A)) {
    direction = -1.0f;
  }
  else if(input_key_down(KEY_D)) {
    direction = 1.0f;
  }

  cam.position.x += direction * speed * delta_time;
}

void camera_follow(Camera& cam, const Vec2& target, const Vec2& offset) {
  cam.position = target + offset;
}

void camera_follow_lerp(Camera& cam, const Vec2& target, const Vec2& offset, const f32 delta) {
  cam.position = vec2_lerp(cam.position, target + offset, delta);
}

Vec2 camera_world_to_screen_space(const Camera& cam, const Vec2& position) {
  Vec4 new_pos = Vec4(position.x, position.y, 0.0f, 1.0f) * cam.view;
  return Vec2(new_pos.x, new_pos.y);
}

Vec2 camera_screen_to_world_space(const Camera& cam, const Vec2& position) {
  // Converting to the NDC coordinates 
  // (converting the given pixel position to a range of [-1, 1]).

  f32 ndc_x = (position.x / cam.view_bounds.x) * 2.0f - 1.0f;
  f32 ndc_y = 1.0f - (position.y / cam.view_bounds.y) * 2.0f; // Flipping the Y-axis, because OpenGL...

  Vec4 world_pos = mat4_inverse(cam.view_proj) * Vec4(ndc_x, ndc_y, 0.0f, 1.0f);

  // Done!
  return Vec2(world_pos.x, world_pos.y);
}

/// Camera functions
/// ----------------------------------------------------------------------

} // End of freya

//////////////////////////////////////////////////////////////////////////

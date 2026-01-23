#include "freya_render.h"

//////////////////////////////////////////////////////////////////////////

namespace freya { // Start of freya

/// ----------------------------------------------------------------------
/// Camera functions

void camera_create(Camera& out_camera, const CameraDesc& desc) {
  out_camera.position = -desc.position;
  out_camera.rotation = desc.rotation;
  out_camera.zoom     = desc.zoom;

  out_camera.sensitivity = 0.1f;
  out_camera.exposure    = 1.0f;
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
  Vec4 new_pos = Vec4(position.x, position.y, 0.0f, 1.0f) * mat4_inverse(cam.view);
  return Vec2(new_pos.x, new_pos.y);
}

/// Camera functions
/// ----------------------------------------------------------------------

} // End of freya

//////////////////////////////////////////////////////////////////////////

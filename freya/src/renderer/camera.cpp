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

Vec2 camera_world_to_screen_space(const Camera& cam, const Vec2 position, const Window* window) {
  // Get the window size
  IVec2 window_size = window_get_size(window); 

  // Transform the given position to NDC

  Vec4 ndc_pos = Vec4(Vec3(position.x, position.y, 0.0f), 1.0f);
  ndc_pos      = cam.view * ndc_pos;

  // Transform the NDC position into screen space

  Vec2 screen_space = Vec2(ndc_pos.x / ndc_pos.w, -ndc_pos.y / ndc_pos.w);
  return (screen_space + 1.0f) / 2.0f * Vec2(window_size);
}

/// Camera functions
/// ----------------------------------------------------------------------

} // End of freya

//////////////////////////////////////////////////////////////////////////

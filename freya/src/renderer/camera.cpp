#include "freya_render.h"

//////////////////////////////////////////////////////////////////////////

namespace freya { // Start of freya

/// ----------------------------------------------------------------------
/// Camera functions

void camera_create(Camera& out_camera, const CameraDesc& desc) {
  out_camera.transform.position    = -desc.position;
  out_camera.transform.rotation    = desc.rotation;
  out_camera.transform.scale       = Vec2(desc.zoom);
  transform_apply(out_camera.transform);

  out_camera.sensitivity = 0.1f;
  out_camera.exposure    = 1.0f;
}

void camera_follow(Camera& cam, const Vec2& target, const Vec2& offset) {
  cam.transform.position = target + offset;
  transform_apply(cam.transform);
}

void camera_follow_lerp(Camera& cam, const Vec2& target, const Vec2& offset, const f32 delta) {
  transform_lerp_position(cam.transform, target + offset, delta);
}

Vec2 camera_world_to_screen_space(const Camera& cam, const Vec2 position, const Window* window) {
  // Get the window size
  IVec2 window_size = window_get_size(window); 

  // Transform the given position to NDC

  Vec4 ndc_pos = Vec4(Vec3(position.x, position.y, 0.0f), 1.0f);
  ndc_pos      = cam.transform.model * ndc_pos;

  // Transform the NDC position into screen space

  Vec2 screen_space = Vec2(ndc_pos.x / ndc_pos.w, -ndc_pos.y / ndc_pos.w);
  return (screen_space + 1.0f) / 2.0f * Vec2(window_size);
}

/// Camera functions
/// ----------------------------------------------------------------------

} // End of freya

//////////////////////////////////////////////////////////////////////////

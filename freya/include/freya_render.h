#pragma once

#include "freya_assets.h"

//////////////////////////////////////////////////////////////////////////

namespace freya { // Start of freya

///---------------------------------------------------------------------------------------------------------------------
/// Consts

/// The maximum amount of zoom the camera can achieve.
const f32 CAMERA_MAX_ZOOM = 180.0f;

/// Consts
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// CameraDesc
struct CameraDesc {
  Vec2 position; 
  
  f32 sensitivity = 0.1f;
  f32 exposure    = 1.0f; 
};
/// CameraDesc
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Camera 
struct Camera {
  Vec2 position; 

  f32 zoom        = 45.0f;
  f32 sensitivity = 0.1f;
  f32 exposure    = 1.0f; 

  Mat4 view, projection, view_projection;
};
/// Camera 
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Camera functions

/// Fill the information in `out_camera` using the given `desc`.
FREYA_API void camera_create(Camera& out_camera, const CameraDesc& desc);

/// Update the projection of the given `cam`.
///
/// @NOTE: This function must be called _every_ frame.
FREYA_API void camera_update(Camera& cam);

/// Have the given `cam` follow the `target`, taking into account the given `offset`.
FREYA_API void camera_follow(Camera& cam, const Vec2& target, const Vec2& offset);

/// Using linear interpolation, have the given `cam` follow `target` by `delta`, taking
/// into account the given `offset`.
FREYA_API void camera_follow_lerp(Camera& cam, const Vec2& target, const Vec2& offset, const f32 delta);

/// Using the given `cam`, convert the world space `position` to screen space coordinates 
/// using the width and height of the given `window`.
FREYA_API Vec2 camera_world_to_screen_space(const Camera& cam, const Vec2& position, const Window* window);

/// Camera functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Renderer functions

/// Initialize the internal data of the renderer.
FREYA_API void renderer_init(Window* window);

/// Shutdown and destroy any resources created by the renderer.
FREYA_API void renderer_shutdown();

/// Setup the renderer for any proceeding render operation, using the given `cam`.
FREYA_API void renderer_begin(const Camera& camera);

/// Sumbit the results of the renderer to the screen.
FREYA_API void renderer_end();

/// Set renderer's clear color to the given `color`.
FREYA_API void renderer_set_clear_color(const Vec4& color);

/// Retrieve the renderer's current clear color.
FREYA_API const Vec4& renderer_get_clear_color();

/// Retrieve the internal `GfxContext` of the global renderer.
FREYA_API GfxContext* renderer_get_context();

/// Queue a texture to be drawn by the end of the frame, using
/// the given `texture_id` at `src` and render into `dest`, tinted with `tint`.
///
/// @NOTE: By default, `tint` is set to `Vec4(1.0f)`.
FREYA_API void renderer_queue_texture(const AssetID& texture_id, const Rect2D& src, const Rect2D& dest, const Vec4& tint = Vec4(1.0f));

/// Queue a texture to be drawn by the end of the frame, using
/// the given `texture_id` at `position` with size of `size` and tinted with `tint`.
///
/// @NOTE: By default, `tint` is set to `Vec4(1.0f)`.
FREYA_API void renderer_queue_texture(const AssetID& texture_id, const Vec2& position, const Vec2& size, const Vec4& tint = Vec4(1.0f));

/// Queue a quad at `position` with a size of `size` and tinted with `color`.
FREYA_API void renderer_queue_quad(const Vec2& position, const Vec2& size, const Vec4& color);

/// Renderer functions
///---------------------------------------------------------------------------------------------------------------------

} // End of freya

//////////////////////////////////////////////////////////////////////////

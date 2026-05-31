#pragma once

#include "freya_assets.h"
#include "freya_timer.h"

//////////////////////////////////////////////////////////////////////////

namespace freya { // Start of freya

///---------------------------------------------------------------------------------------------------------------------
/// Consts

/// The maximum amount of zoom the camera can achieve.
const f32 CAMERA_MAX_ZOOM = 180.0f;

/// The maximum amount of particles tha can be emitted per emitter.
const sizei PARTICLES_MAX = 1024;

/// Consts
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// ParticleDistributionType
enum ParticleDistributionType {
  DISTRIBUTION_RANDOM = 0, 
  DISTRIBUTION_SQUARE,
  DISTRIBUTION_CIRCULAR,
};
/// ParticleDistributionType
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Function signatures

struct PostProcessPass;

using OnPassPrepareFn = std::function<void(PostProcessPass* pass)>;
using OnPassResizeFn  = std::function<void(PostProcessPass* pass, const IVec2& new_size)>;

/// Function signatures
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// PostProcessPass
struct PostProcessPass {
  OnPassPrepareFn prepare_func = nullptr;
  OnPassResizeFn resize_func   = nullptr;

  IVec2 frame_size;
  Color clear_color;

  // GfxFramebufferDesc frame_desc;
  // GfxFramebuffer* frame;

  Array<Texture, 8> outputs;
  sizei outputs_count = 0;

  PostProcessPass* previous = nullptr;
  String debug_name;
};
/// PostProcessPass
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// PostProcessPassDesc
struct PostProcessPassDesc {
  OnPassPrepareFn prepare_func = nullptr;
  OnPassResizeFn resize_func   = nullptr;

  IVec2 frame_size          = IVec2(0);
  Color clear_color         = COLOR_WHITE;
  AssetID shader_context_id = {};
  AssetGroupID asset_group  = {};

  u32 clear_flags;
  DynamicArray<sg_pixel_format> attachments;

  String debug_name = "DEBUG";
};
/// PostProcessPassDesc
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// CameraDesc
struct CameraDesc {
  /// The starting position of the camera.
  Vec2 position; 

  /// The view bounds of the camera which 
  /// will be passed to the renderer. 
  IVec2 view_bounds;

  /// The starting rotation of the camera.
  ///
  /// @NOTE: The default value is `0.0f`.
  f32 rotation = 0.0f;

  /// The zoom factor of the camera.
  ///
  /// @NOTE: The default value is `1.0f`.
  f32 zoom     = 1.0f;
 
  /// The HDR exposure of the camera.
  ///
  /// @NOTE: The default value is `1.0f`.
  f32 exposure = 1.0f; 
};
/// CameraDesc
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Camera 
struct Camera {
  Vec2 position;
  IVec2 view_bounds;

  f32 zoom, rotation;

  Mat4 view       = Mat4(1.0f);
  Mat4 projection = Mat4(1.0f);
  Mat4 view_proj  = Mat4(1.0f);

  f32 exposure = 1.0f; 
};
/// Camera 
///---------------------------------------------------------------------------------------------------------------------

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

/// ----------------------------------------------------------------------
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
/// ----------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// ParticleEmitterDesc
struct ParticleEmitterDesc {
  /// The velocity of each particle that will be applied 
  /// in the update loop. 
  Vec2 velocity;

  /// The amount of particles to emit. 
  ///
  /// @NOTE: This variable CANNOT exceed `PARTICLES_CPU_MAX`.
  i32 count; 

  /// The unit scale of each particle in the system. 
  ///
  /// @NOTE: The default scale is set to `Vec2(1.0f, 1.0f)`.
  Vec2 scale                            = Vec2(1.0f);

  /// The texture ID to be used when rendering the particles. 
  ///
  /// @NOTE: This can be set to be invalid to render the 
  /// default texture.
  AssetID texture_id                    = {};

  /// The color/tint of the particles when rendered. 
  ///
  /// @NOTE: The default values is `Vec4(1.0f, 1.0f, 1.0f, 1.0f)`.
  Vec4 color                            = Vec4(1.0f);

  /// The maximum amount of time a particle can 
  /// live for after being activated.
  ///
  /// @NOTE: The default lifetime is set to `2.5f`.
  f32 lifetime                          = 2.5f;

  /// The gravity contributor of each particle in the system.
  ///
  /// @NOTE: The default gravity is set to `240.0f`.
  f32 gravity_factor                    = 240.0f;

  /// Defines how the particles will be distributed 
  /// when emitted. 
  ///
  /// @NOTE: The default distribution is set to `DISTRIBUTION_RANDOM`.
  ParticleDistributionType distribution = DISTRIBUTION_RANDOM;

  /// The area or radius of the distribution being applied. 
  /// The radius will act differently depending on the specific distribution. 
  ///
  /// For example, for the `DISTRIBUTION_RANDOM` type, the radius will 
  /// be the maxium value of the random function. While the negation of 
  /// the radius will be the minimum value of the random function.
  ///
  /// @NOTE: The default distribution radius is set to `1.0f`.
  f32 distribution_radius               = 1.0f;
};
/// ParticleEmitterDesc
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// ParticleEmitter 
struct ParticleEmitter {
  Vec2 initial_scale    = Vec2(0.0f);
  Vec2 initial_velocity = Vec2(0.0f);

  Transform transforms[PARTICLES_MAX];
  Vec2 forces[PARTICLES_MAX];
  Vec2 velocities[PARTICLES_MAX];

  i32 particles_count = 0;
  Timer lifetime; 

  Texture texture; 
  Vec4 color;
  
  f32 distribution_radius               = 1.0f;
  ParticleDistributionType distribution = DISTRIBUTION_RANDOM;

  f32 gravity_factor = 0.0f; 
  bool is_active     = false;
};
/// ParticleEmitter 
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// PostProcess functions

/// Allocate and initialize a `PostProcessPass`, using the information given from `desc`.
FREYA_API PostProcessPass* post_process_create(const PostProcessPassDesc& desc);

/// Create a pre-defined blur pass, using the given `window`.
FREYA_API PostProcessPass* post_process_define_blur(Window* window);

/// Create a pre-defined greyscale pass, using the given `window`.
FREYA_API PostProcessPass* post_process_define_greyscale(Window* window);

/// Create a pre-defined vignette pass, using the given `window` and `intensity` of the vignette.
FREYA_API PostProcessPass* post_process_define_vignette(Window* window, const f32 intensity = 12.0f);

/// Prepare the given `pass` for rendering.
///
/// @NOTE: This function does not need to be called by any client code. 
/// It will only be used internally by the renderer.
FREYA_API void post_process_prepare(PostProcessPass* pass);

/// Destroy the given `pass`, reclaiming any allocated memory.
FREYA_API void post_process_destroy(PostProcessPass* pass);

/// PostProcess functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Camera functions

/// Fill the information in `out_camera` using the given `desc`.
FREYA_API void camera_create(Camera& out_camera, const CameraDesc& desc);

/// Move the given `camera` by `speed` on the X and Y axis, using 
/// the WASD keys.
/// 
/// @NOTE: This is just a helper function to get camera movement.
FREYA_API void camera_move_top_down(Camera& cam, const Vec2& speed, const f32 delta_time);

/// Move the given `camera` by `speed` on the X, using the A and D keys.
/// 
/// @NOTE: This is just a helper function to get camera movement.
FREYA_API void camera_move_side_scroller(Camera& cam, const f32 speed, const f32 delta_time);

/// Have the given `cam` follow the `target`, taking into account the given `offset`.
FREYA_API void camera_follow(Camera& cam, const Vec2& target, const Vec2& offset);

/// Using linear interpolation, have the given `cam` follow `target` by `delta`, taking
/// into account the given `offset`.
FREYA_API void camera_follow_lerp(Camera& cam, const Vec2& target, const Vec2& offset, const f32 delta);

/// Using the given `cam`, convert the world space `position` to screen space coordinates.
FREYA_API Vec2 camera_world_to_screen_space(const Camera& cam, const Vec2& position);

/// Using the given `cam`, convert the screen space `position` to world space coordinates.
///
/// @NOTE: The bounds size will be taken from `cam.view_bounds`.
FREYA_API Vec2 camera_screen_to_world_space(const Camera& cam, const Vec2& position);

/// Camera functions
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

///---------------------------------------------------------------------------------------------------------------------
/// ParticleEmitter functions

/// Create a particle emitter `out_emitter` using the information in `desc`.
FREYA_API void particle_emitter_create(ParticleEmitter& out_emitter, const ParticleEmitterDesc& desc);

/// Create a particle emitter `out_emitter` using the config file given in `config_id`.
FREYA_API void particle_emitter_create(ParticleEmitter& out_emitter, const AssetID& config_id);

/// A physics update of each particle in the given `emitter` using the scale of `delta_time`. 
FREYA_API void particle_emitter_update(ParticleEmitter& emitter, const f32 delta_time); 

/// Emit the particles of `emitter` at `position`.
FREYA_API void particle_emitter_emit(ParticleEmitter& emitter, const Vec2& position);

/// Reset the given `emitter` to its initial state, with all particles being positioned at `position`.
FREYA_API void particle_emitter_reset(ParticleEmitter& emitter, const Vec2& position);

/// ParticleEmitter functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Renderer functions

/// Initialize the internal data of the renderer.
FREYA_API void renderer_init(Window* window);

/// Shutdown and destroy any resources created by the renderer.
FREYA_API void renderer_shutdown();

/// Setup the renderer for any proceeding render operation, using the given `cam`.
FREYA_API void renderer_begin(Camera& camera);

/// Sumbit the results of the renderer to the screen.
FREYA_API void renderer_end();

/// Set renderer's clear color to the given `color`.
FREYA_API void renderer_set_clear_color(const Color& color);

/// Retrieve the renderer's current clear color.
FREYA_API const Color& renderer_get_clear_color();

/// Push the given `pass` to the back of the current post-process chain.
FREYA_API void renderer_push_post_process(PostProcessPass* pass);

/// Pop the back of the post-process chain, removing it from the chain, and returning it.
///
/// @NOTE: The popped pass will still be valid, but it just won't be processed 
/// at the end of the frame anymore.
FREYA_API PostProcessPass* renderer_pop_post_process();

/// Queue a texture to be drawn by the end of the frame, using
/// the given `texture` at `src` and render into `dest`, rotated by `rotation`, tinted with `tint`.
///
/// @NOTE: By default, `tint` is set to `Color(1.0f)`.
FREYA_API void renderer_queue_texture(const Texture& texture, 
                                      const Rect2D& src, 
                                      const Rect2D& dest, 
                                      const f32 rotation = 0.0f,
                                      const Color& tint  = Color(1.0f));

/// Queue a texture to be drawn by the end of the frame, using
/// the given `texture`, and transform, with `tint` color.
///
/// @NOTE: By default, `tint` is set to `Color(1.0f)`.
FREYA_API void renderer_queue_texture(const Texture& texture, const Transform& transform, const Color& tint = Color(1.0f));

/// Queue a quad using `transform` with a `color`.
FREYA_API void renderer_queue_quad(const Transform& transform, const Color& color);

/// Queue a simple line starting from `start` till `end` with a `color`.
FREYA_API void renderer_queue_line(const Vec2& start, const Vec2& end, const Color& color);

/// Queue a simple point at `position` with size `size` with a `color`.
FREYA_API void renderer_queue_point(const Vec2& position, f32 size, const Color& color);

/// Queue a three-point triangle with points `p1`, `p2`, and `p3` with a `color`.
FREYA_API void renderer_queue_triangle(const Vec2& p1, const Vec2& p2, const Vec2& p3, const Color& color);

/// Queue an array of triangle strips with `vertices` at `transform` tinted with `color`.
FREYA_API void renderer_queue_triangles_strip(const Transform& transform, 
                                              const DynamicArray<Vec2>& vertices, 
                                              const Color& color);

/// Queue an animation using the given `animation`, transformed with `transform` with a `tint`.
///
/// @NOTE: By default, `tint` is set to `Color(1.0f)`.
FREYA_API void renderer_queue_animation(const Animation& anim, const Transform& transform, const Color& tint = Color(1.0f));

/// Queue particles using the given `emitter`.
FREYA_API void renderer_queue_particles(const ParticleEmitter& emitter);

/// Renderer functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// UI renderer functions

/// Initialize the UI renderer.
FREYA_API bool ui_renderer_init();

/// Shutdown and reclaim all the memory allocated by the UI renderer.
FREYA_API void ui_renderer_shutdown();

/// Begin the rendering process of the UI renderer.
FREYA_API void ui_renderer_begin();

/// End the rendering process of the UI renderer. 
FREYA_API void ui_renderer_end();

/// Set the internal asset group of the UI renderer to the given `group_id`.
/// 
/// @NOTE: This is essential, since the renderer will refer (by name, usually) 
/// to already-loaded assets. This function _MUST_ be called at least once on initialization.
FREYA_API void ui_renderer_set_asset_group(const AssetGroupID& group_id);

/// Set the internal font of the UI renderer to the font retrieved 
/// from the internal asset group of the UI renderer using the given `font_name`.
///
/// @NOTE: This function _MUST_ be called only after setting the internal 
/// asset group of the UI renderer (see `ui_renderer_set_asset_group`).
/// In addition, this function _MUST_ be called at least once on initialization.
FREYA_API bool ui_renderer_set_font(const String& font_name);

/// UI renderer functions
///---------------------------------------------------------------------------------------------------------------------

} // End of freya

//////////////////////////////////////////////////////////////////////////

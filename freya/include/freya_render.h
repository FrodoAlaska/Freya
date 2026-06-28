#pragma once

#include "freya_entity.h"

//////////////////////////////////////////////////////////////////////////

namespace freya { // Start of freya

///---------------------------------------------------------------------------------------------------------------------
/// Consts

/// The maximum amount of zoom the camera can achieve.
const f32 CAMERA_MAX_ZOOM    = 180.0f;

/// The maximum amount of particles tha can be emitted per emitter.
const u32 PARTICLES_MAX      = 1024;

/// The maximum amount of render targets a post-process pass can have. 
const u32 RENDER_TARGETS_MAX = 8; 

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
/// PostProcessPassDesc
struct PostProcessPassDesc {
  OnPassPrepareFn prepare_func = nullptr;
  OnPassResizeFn resize_func   = nullptr;

  IVec2 frame_size = IVec2(-1); // Only used in non depth/depth-stencil attachments
  f32 depth_clear  = 1.0f;      // Only used in depth/depth-stencil attachments

  Color clear_color     = COLOR_WHITE;
  AssetID shader_id     = {};
  AssetGroupID group_id = {};

  i32 samples_count = 1;

  DynamicArray<sg_pixel_format> attachments;
  String debug_name = "DEBUG";
};
/// PostProcessPassDesc
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// PostProcessPass
struct PostProcessPass {
  Window* window = nullptr;

  OnPassPrepareFn prepare_func = nullptr;
  OnPassResizeFn resize_func   = nullptr;

  IVec2 frame_size;
  Color clear_color;
  f32 depth_clear;

  sg_pipeline pipeline = {};

  sg_pass_action action = {};
  sg_pass pass          = {};

  DynamicArray<sg_view> attachments;
  Array<sg_view, RENDER_TARGETS_MAX> outputs;

  u32 outputs_count = 0;

  PostProcessPass* previous = nullptr;
  String debug_name;
};
/// PostProcessPass
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

  /// The bounds that the particles can traverse in. Particles 
  /// cannot go beyond these bounds at all.
  ///
  /// @NOTE: By default, this is set to `Vec2(1024.0f)`.
  Vec2 bounds                           = Vec2(1024.0f);

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
  Vec2 bounds           = Vec2(0.0f);

  Transform transforms[PARTICLES_MAX];
  Vec2 forces[PARTICLES_MAX];
  Vec2 velocities[PARTICLES_MAX];

  Vec2 position = Vec2(0.0f);

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

/// Allocate a `PostProcessPass` object.
FREYA_API PostProcessPass* post_process_allocate(Window* window);

/// Initialize a given `pass` post-process object using the information given from `desc`.
FREYA_API void post_process_init(PostProcessPass* pass, const PostProcessPassDesc& desc);

/// Allocate and initialize a `PostProcessPass`, using the information given from `desc`, 
/// effectively the same as calling both `post_process_allocate` and `post_process_init`.
FREYA_API PostProcessPass* post_process_create(Window* window, const PostProcessPassDesc& desc);

/// Create a pre-defined blur pass, using the given `window`.
FREYA_API PostProcessPass* post_process_define_blur(Window* window);

/// Create a pre-defined greyscale pass, using the given `window`.
FREYA_API PostProcessPass* post_process_define_greyscale(Window* window);

/// Create a pre-defined vignette pass, using the given `window` and `intensity` of the vignette.
FREYA_API PostProcessPass* post_process_define_vignette(Window* window, const f32 intensity = 12.0f);

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
/// ParticleEmitter functions

/// Create a particle emitter `out_emitter` using the information in `desc`.
FREYA_API void particle_emitter_create(ParticleEmitter& out_emitter, const ParticleEmitterDesc& desc);

/// Create a particle emitter `out_emitter` using the config file given in `config_id`.
FREYA_API void particle_emitter_create(ParticleEmitter& out_emitter, const AssetID& config_id);

/// A physics update of each particle in the given `emitter` using the scale of `delta_time`. 
FREYA_API void particle_emitter_update(ParticleEmitter& emitter, const f32 delta_time); 

/// Emit the particles of `emitter` at `position`.
FREYA_API void particle_emitter_emit(ParticleEmitter& emitter, const Vec2& position);

/// Reset the given `emitter` to its initial state.
FREYA_API void particle_emitter_reset(ParticleEmitter& emitter);

/// ParticleEmitter functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Renderer functions

/// Initialize the internal data of the renderer.
FREYA_API void renderer_init(Window* window);

/// Shutdown and destroy any resources created by the renderer.
FREYA_API void renderer_shutdown();

/// Apply the given `group_id` to be used internally in the renderer. 
///
/// @NOTE: If no custom `AssetGroupID` is applied, the renderer 
/// will use the default `ASSET_CACHE_ID` group instead.
FREYA_API void renderer_apply_asset_group(const AssetGroupID& group_id);

/// Apply the internal font of the renderer to the font retrieved 
/// from given asset ID `font_id`, returning `true` on success and `false` on failure.
///
/// @NOTE: This function _MUST_ be called before 
FREYA_API bool renderer_apply_font(const AssetID& font_id);

/// Sumbit the given `world` to be renderer in this frame. 
///
/// @NOTE: This function _MUST_ be called at least once during 
/// the application's runtime. Otherwise, when the engine calls `renderer_prepare`, 
/// the application will assert due to an invalid current world.
FREYA_API void renderer_sumbit_world(EntityWorld* world);

/// Set up the internal render buffers of the renderer using the 
/// `EntityWorld` that was sumbitted using `renderer_sumbit_world`.
///
/// @NOTE: The application does NOT need to call this function. This is 
/// only required to be called by the engine.
FREYA_API void renderer_prepare();

/// Commit and render all of the objects sumbitted to the renderer 
/// and present the last image to the swapchain.
///
/// @NOTE: The application does NOT need to call this function. This is 
/// only required to be called by the engine.
FREYA_API void renderer_commit();

/// Push the given `pass` to the back of the current post-process chain.
FREYA_API void renderer_push_post_process(PostProcessPass* pass);

/// Pop the back of the post-process chain, removing it from the chain, and returning it.
///
/// @NOTE: The popped pass will still be valid, but it just won't be processed 
/// at the end of the frame anymore.
FREYA_API PostProcessPass* renderer_pop_post_process();

/// Set renderer's clear color to the given `color`.
FREYA_API void renderer_set_clear_color(const Color& color);

/// Set whether the renderer should sort the renderable items or not.
FREYA_API void renderer_set_sort(bool sort);

/// Retrieve the renderer's current clear color.
FREYA_API const Color& renderer_get_clear_color();

/// Retrieve the number of created post-process passes
FREYA_API u32 renderer_get_post_process_count();

/// Retrieve a default platform-specific swapchain to give to any passes
FREYA_API sg_swapchain renderer_get_default_swapchain();

/// Retrieve the `AssetGroupID` the renderer is currently using.
FREYA_API AssetGroupID& renderer_get_asset_group_id();

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

} // End of freya

//////////////////////////////////////////////////////////////////////////

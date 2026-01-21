#pragma once

#include "freya_assets.h"

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
/// Color
using Color = Vec4;
/// Color
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Color consts

/// Predefined colors for ease-of-use.

const Color COLOR_WHITE  = Color(1.0f);
const Color COLOR_BLACK  = Color(0.0f, 0.0f, 0.0f, 1.0f);
const Color COLOR_RED    = Color(1.0f, 0.0f, 0.0f, 1.0f);
const Color COLOR_GREEN  = Color(0.0f, 1.0f, 0.0f, 1.0f);
const Color COLOR_BLUE   = Color(0.0f, 0.0f, 1.0f, 1.0f);
const Color COLOR_YELLOW = Color(1.0f, 1.0f, 0.0f, 1.0f);
const Color COLOR_CYAN   = Color(0.0f, 1.0f, 1.0f, 1.0f);
const Color COLOR_PURPLE = Color(1.0f, 0.0f, 1.0f, 1.0f);

/// Color consts
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// CameraDesc
struct CameraDesc {
  Vec2 position; 
  
  f32 sensitivity = 0.1f;
  f32 exposure    = 1.0f; 

  f32 zoom     = 1.0f;
  f32 rotation = 0.0f;
};
/// CameraDesc
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Camera 
struct Camera {
  Transform transform; 

  f32 sensitivity = 0.1f;
  f32 exposure    = 1.0f; 
};
/// Camera 
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// ShaderContext
struct ShaderContext {
  /// The underlying shader pointer of the context.
  GfxShader* shader = nullptr; 

  /// A cache of uniforms where the key is the name of 
  /// the uniform in the shader and the value is the 
  /// uniform's location in the shader.
  HashMap<String, i32> uniforms_cache;
};
/// ShaderContext
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Font
struct Font {
  /// @NOTE:
  ///
  /// The library we currently use for UI (RmlUi) uses FreeType to load 
  /// the fonts and deal with all that logic. All it needs from us, however, 
  /// is the byte data loaded from a truetype font. And `font_data` is just that. 
  ///
  /// This is clearly not a good approach. But, in the future, we will make RmlUi 
  /// rely on _our_ font data. For now, though, this is how it works.
  ///

  String name;
  DynamicArray<u8> font_data;
};
/// Font
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
  GfxTexture* texture; 
  Vec2 frame_size;

  i32 current_frame, frames_count;
  i32 direction, start_row;

  f32 counter, flip_speed;
  bool is_active, can_loop, can_alternate;

  Rect2D src_rect;
};
/// Animation
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// ParticleEmitterDesc
struct ParticleEmitterDesc {
  /// The starting position of the particles.
  Vec2 position; 

  /// The unit scale of each particle in the system. 
  ///
  /// @NOTE: The default scale is set to `Vec2(1.0f, 1.0f)`.
  Vec2 scale;

  /// The velocity of each particle that will be applied 
  /// in the update loop. 
  Vec2 velocity;

  /// The amount of particles to emit. 
  ///
  /// @NOTE: This variable CANNOT exceed `PARTICLES_CPU_MAX`.
  sizei count; 

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
  Vec2 initial_position = Vec2(0.0f);
  Vec2 initial_scale    = Vec2(0.0f);
  Vec2 initial_velocity = Vec2(0.0f);

  Transform transforms[PARTICLES_MAX];
  Vec2 forces[PARTICLES_MAX];
  Vec2 velocities[PARTICLES_MAX];

  sizei particles_count = 0;
  Timer lifetime; 

  GfxTexture* texture; 
  Vec4 color;
  
  f32 distribution_radius               = 1.0f;
  ParticleDistributionType distribution = DISTRIBUTION_RANDOM;

  f32 gravity_factor = 0.0f; 
  bool is_active     = false;
};
/// ParticleEmitter 
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Color functions

/// Return a new RGB `Color` from the given `hex_color`. 
///
/// @NOTE: The given hex color should be arranged as follows: `0xRRGGBBAA`.
FREYA_API Color color_hex_to_rgb(const u32 hex_color);

/// Return a new HEX value from the given `rgb` color.
///
/// @NOTE: The returned hex color will be arranged as follows: `0xRRGGBBAA`.
FREYA_API u32 color_rgb_to_hex(const Color& rgb);

/// Lerp the given `color` towards `other` by `delta`.
FREYA_API void color_lerp(Color& color, const Color& other, const f32 delta);

/// Use smoothstep to interpolate the given `color` towards `other` by `amount`.
FREYA_API void color_slerp(Color& color, const Color& other, const Color& amount);

/// Color functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Camera functions

/// Fill the information in `out_camera` using the given `desc`.
FREYA_API void camera_create(Camera& out_camera, const CameraDesc& desc);

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
/// ShaderContext functions

/// Cache the location of the uniform with the name `uniform_name` to the given `ctx`.
/// 
/// @NOTE: If the uniform's name is not found within the context, the function will throw a warning. 
FREYA_API void shader_context_cache_uniform(ShaderContext* ctx, const i8* uniform_name);

/// Set a uniform of type `i32` with the name `uniform_name` in `ctx` to the given `value`. 
FREYA_API void shader_context_set_uniform(ShaderContext* ctx, const String& uniform_name, const i32 value);

/// Set a uniform of type `f32` with the name `uniform_name` in `ctx` to the given `value`. 
FREYA_API void shader_context_set_uniform(ShaderContext* ctx, const String& uniform_name, const f32 value);

/// Set a uniform of type `Vec2` with the name `uniform_name` in `ctx` to the given `value`. 
FREYA_API void shader_context_set_uniform(ShaderContext* ctx, const String& uniform_name, const Vec2& value);

/// Set a uniform of type `Vec3` with the name `uniform_name` in `ctx` to the given `value`. 
FREYA_API void shader_context_set_uniform(ShaderContext* ctx, const String& uniform_name, const Vec3& value);

/// Set a uniform of type `Vec4` with the name `uniform_name` in `ctx` to the given `value`. 
FREYA_API void shader_context_set_uniform(ShaderContext* ctx, const String& uniform_name, const Vec4& value);

/// Set a uniform of type `Mat4` with the name `uniform_name` in `ctx` to the given `value`. 
FREYA_API void shader_context_set_uniform(ShaderContext* ctx, const String& uniform_name, const Mat4& value);

/// Set an array uniforms of type `i32` with the name `uniform_name` in `ctx` to the given `values` with `count` elements. 
FREYA_API void shader_context_set_uniform_array(ShaderContext* ctx, const String& uniform_name, const i32* values, const sizei count);

/// Set an array uniforms of type `f32` with the name `uniform_name` in `ctx` to the given `values` with `count` elements. 
FREYA_API void shader_context_set_uniform_array(ShaderContext* ctx, const String& uniform_name, const f32* values, const sizei count);

/// ShaderContext functions
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
/// ParticleEmitter functions

/// Create a particle emitter `out_emitter`, using the information in `desc`.
FREYA_API void particle_emitter_create(ParticleEmitter& out_emitter, const ParticleEmitterDesc& desc);

/// A physics update of each particle in the given `emitter` using the scale of `delta_time`. 
FREYA_API void particle_emitter_update(ParticleEmitter& emitter, const f32 delta_time); 

/// Emit the particles of `emitter`.
FREYA_API void particle_emitter_emit(ParticleEmitter& emitter);

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

/// Setup the renderer for any proceeding render operation, using the given `cam`.
FREYA_API void renderer_begin(const Camera& camera);

/// Sumbit the results of the renderer to the screen.
FREYA_API void renderer_end();

/// Set renderer's clear color to the given `color`.
FREYA_API void renderer_set_clear_color(const Color& color);

/// Retrieve the renderer's current clear color.
FREYA_API const Color& renderer_get_clear_color();

/// Retrieve the internal `GfxContext` of the global renderer.
FREYA_API GfxContext* renderer_get_context();

/// Queue a texture to be drawn by the end of the frame, using
/// the given `texture` at `src` and render into `dest`, tinted with `tint`.
///
/// @NOTE: By default, `tint` is set to `Color(1.0f)`.
FREYA_API void renderer_queue_texture(GfxTexture* texture, const Rect2D& src, const Rect2D& dest, const Color& tint = Color(1.0f));

/// Queue a texture to be drawn by the end of the frame, using
/// the given `texture`, and transform, with `tint` color.
///
/// @NOTE: By default, `tint` is set to `Color(1.0f)`.
FREYA_API void renderer_queue_texture(GfxTexture* texture, const Transform& transform, const Color& tint = Color(1.0f));

/// Queue a quad using `transform` with a `color`.
FREYA_API void renderer_queue_quad(const Transform& transform, const Color& color);

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
FREYA_API bool ui_renderer_init(GfxContext* gfx);

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

#pragma once

#include "freya_physics.h"
#include "freya_noise.h"
#include "freya_animation.h"
#include "freya_ui.h"

//////////////////////////////////////////////////////////////////////////

namespace freya { // Start of freya

/// ----------------------------------------------------------------------
/// Forward declarations

struct TileMap;

struct CameraDesc;
struct Camera;

struct ParticleEmitterDesc;
struct ParticleEmitter;

/// Forward declarations
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// EntityID
using EntityID = entt::entity;
/// EntityID
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// EntityWorld
using EntityWorld = entt::registry;
/// EntityWorld
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Consts

/// Used to indicate an invalid entity ID.
const EntityID ENTITY_NULL = entt::null;

/// Consts
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Callbacks

/// Called when the physics body of `entt` is collided with `other` in the physics world, 
/// passing in the generated `normal` from the collision, and `user_data`.
using OnCollisionFn = std::function<void(EntityWorld& world, EntityID& entt, EntityID& other, const Vec2& normal, void* user_data)>;

/// Called when the timer of `entt` has runout, passing in `user_data`.
using OnTimerRunoutFn = std::function<void(EntityWorld& world, EntityID& entt, void* user_data)>;

/// Callbacks
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// TagComponent
struct TagComponent {
  String tag;
};
/// TagComponent
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// TimerComponent
struct TimerComponent {
  /// The internal timer that can be used with `timer_*` functions.
  Timer timer; 

  /// The callback that will be initiated when the timer runs out.
  OnTimerRunoutFn runout_func = nullptr;

  /// The internal user data that will be passed to `runout_func` 
  /// upon initiation.
  void* user_data             = nullptr;
};
/// TimerComponent
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// SpriteComponent
struct SpriteComponent {
  /// The texture that will 
  /// be given to the render command.
  ///
  /// @NOTE: If this texture is left as a 
  /// `nullptr`, the default texture will be used.
  Texture texture = {}; 

  /// The color/tint that will be used 
  /// in the render command. 
  ///
  /// @NOTE: This is `COLOR_WHITE` by default.
  Vec4 color      = COLOR_WHITE;

  /// The layer associated with the sprite. 
  /// `-1` indicates that the sprite will be 
  /// rendered at the very back. The higher 
  /// the value, the farther to the front the sprite will be drawn.
  ///
  /// @NOTE: By default, this value is `-1`.
  i32 layer       = -1;
};
/// SpriteComponent
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// TileSpriteComponent
struct TileSpriteComponent {
  /// The texture atlas that will be used 
  /// to source from while rendering.
  Texture texture_atlas = {}; 

  /// The source rect that will be used 
  /// with the texture atlas while rendering. 
  Rect2D source_rect    = {};

  /// The color/tint that will be used 
  /// in the render command. 
  ///
  /// @NOTE: This is `COLOR_WHITE` by default.
  Vec4 color            = COLOR_WHITE;

  /// The layer associated with the sprite. 
  /// `-1` indicates that the sprite will be 
  /// rendered at the very back. The higher 
  /// the value, the farther to the front the sprite will be drawn.
  ///
  /// @NOTE: By default, this value is `-1`.
  i32 layer             = -1;
};
/// TileSpriteComponent
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// StaticBodyComponent
struct StaticBodyComponent {
  /// The internal handle of the physics body, 
  /// to be used with `physics_body_*` functions.
  PhysicsBodyID body; 

  /// Function pointers that will be called either 
  /// on collision enter or collision exit.
  
  OnCollisionFn enter_func = nullptr;
  OnCollisionFn exit_func  = nullptr;
};
/// StaticBodyComponent
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// DynamicBodyComponent
struct DynamicBodyComponent {
  /// The internal handle of the physics body, 
  /// to be used with `physics_body_*` functions.
  PhysicsBodyID body; 

  /// Function pointers that will be called either 
  /// on collision enter or collision exit.
  
  OnCollisionFn enter_func = nullptr;
  OnCollisionFn exit_func  = nullptr;
};
/// DynamicBodyComponent
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// AnimationComponent
struct AnimationComponent {
  /// Animation itself that will be 
  /// updated and rendered.
  Animation animation; 

  /// The tint that will be used 
  /// in the render command. 
  Vec4 tint;
};
/// AnimationComponent
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// EntityWorld functions

/// Clear (and thereby destroy) the given `world` of any 
/// entities and their components.
FREYA_API void entity_world_clear(EntityWorld& world);

/// Update all the components of `world` in a data-oriented manner, using 
/// `delta_time` as the time scale. 
///
/// @NOTE: This function _MUST_ be called only once per frame. 
FREYA_API void entity_world_update(EntityWorld& world, const f32 delta_time);

/// EntityWorld functions
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// EntityID functions

/// Create an return a new entity in the given `world`, with `position`, 
/// `scale`, and `rotation` as its transform properties.
FREYA_API EntityID entity_create(EntityWorld& world,
                                 const Vec2& position, 
                                 const Vec2& scale  = Vec2(1.0f), 
                                 const f32 rotation = 0.0f);

/// Destroy the given `entt` and remove it and its components from `world`.
FREYA_API void entity_destroy(EntityWorld& world, EntityID& entt);

/// Add a generic component `Comp` with `Args` initialization arguments 
/// to the given `entt` in the respective `world`.
template<typename Comp, typename... Args>
FREYA_API Comp& entity_add_component(EntityWorld& world, EntityID& entt, Args&&... args) {
  return world.emplace<Comp>(entt, std::forward<Args>(args)...);
}

/// A helper function to add a camera to `entt`, using the information in `desc`.
///
/// @NOTE: The position of the given `desc` will be set inside the function
/// using the current position of `entt`.
/// However, the rest of the memebers of `desc` must be filled by the caller.
FREYA_API Camera& entity_add_camera(EntityWorld& world, EntityID& entt, CameraDesc& desc);

/// A helper function to add an audio source to `entt`, using the information 
/// in `desc`, and the `audio_buffer_id` as the main buffer.
///
/// @NOTE: The position of the given `desc` will be set inside the function
/// using the current position of `entt`.
/// However, the rest of the memebers of `desc` must be filled by the caller.
FREYA_API AudioSourceID& entity_add_audio_source(EntityWorld& world, 
                                                 EntityID& entt, 
                                                 AudioSourceDesc& desc, 
                                                 const AssetID& audio_buffer_id);

/// A helper function to add a noise generator to `entt`, using the information 
/// in `desc`, mirroring the `noise_generator_create` function.
FREYA_API NoiseGenerator* entity_add_noise_generator(EntityWorld& world, EntityID& entt, const NoiseGeneratorDesc& desc);

/// A helper function to add a tile map component to `entt`, using the given 
/// `start_pos`, `tile_size`, and `tiles_count`, mirroring the `tilemap_create` function.
FREYA_API TileMap& entity_add_tilemap(EntityWorld& world, 
                                      EntityID& entt, 
                                      const Vec2& start_pos, 
                                      const Vec2& tile_size, 
                                      const IVec2& tiles_count);

/// A helper function to add a UI context to `entt` using the given `name` and `view_bounds`.
FREYA_API UIContext* entity_add_ui_context(EntityWorld& world, EntityID& entt, const String& name, const IVec2& view_bounds);

/// A helper function to add a UI text to `entt`, using the information 
/// in `desc`, mirroring the `ui_text_create` function.
///
/// @NOTE: The position of the given `entt` will be used as the 
/// `offset` member of the `desc`.
FREYA_API UIText& entity_add_text(EntityWorld& world, EntityID& entt, UITextDesc& desc);

/// A helper function to add a particle emitter to `entt` using the information 
/// in `desc` or in a config file given in `config_id`.
FREYA_API ParticleEmitter& entity_add_particle_emitter(EntityWorld& world, EntityID& entt, const ParticleEmitterDesc& desc);
FREYA_API ParticleEmitter& entity_add_particle_emitter(EntityWorld& world, EntityID& entt, const AssetID& config_id);

/// A helper function to add a tag to `entt` using the given `tag`.
FREYA_API TagComponent& entity_add_tag(EntityWorld& world, EntityID& entt, const String& tag);

/// A helper function to add a timer component to `entt`, using the given `desc`,
/// mirroring the `timer_create` function, and the `runout_func` and `user_data`.
FREYA_API TimerComponent& entity_add_timer(EntityWorld& world, 
                                           EntityID& entt, 
                                           const TimerDesc& desc, 
                                           const OnTimerRunoutFn& runout_func, 
                                           void* user_data = nullptr);

/// A helper function to add a sprite component to `entt`, using the given
/// `texture_id`, `color`, and `layer` to give to the render command.
FREYA_API SpriteComponent& entity_add_sprite(EntityWorld& world, 
                                             EntityID& entt, 
                                             const AssetID& texture_id, 
                                             const Vec4& color = Vec4(1.0f), 
                                             const i32 layer   = -1);

/// A helper function to add a tile sprite component to `entt`, using the given
/// `texture_id`, `source`, `color`, and `layer` to give to the render command.
FREYA_API TileSpriteComponent& entity_add_tile_sprite(EntityWorld& world, 
                                                      EntityID& entt, 
                                                      const AssetID& texture_id,
                                                      const Rect2D& source, 
                                                      const Vec4& color = Vec4(1.0f), 
                                                      const i32 layer   = -1);

/// A helper function to add a static body to `entt`, using the information 
/// in `desc`, and `enter_func`, and `exit_func`, to call later on collision events. 
///
/// @NOTE: The position, rotation, type, and user data of the given `desc` will be
/// set inside the function using the transform of `entt` and its ID respectively.
/// The type, too, will be set as `PHYSICS_BODY_STATIC`.
/// However, the rest of the memebers of `desc` must be filled by the caller.
FREYA_API StaticBodyComponent& entity_add_static_body(EntityWorld& world, 
                                                      EntityID& entt, 
                                                      PhysicsBodyDesc& desc, 
                                                      const OnCollisionFn& enter_func = nullptr, 
                                                      const OnCollisionFn& exit_func  = nullptr);

/// A helper function to add a dynamic body to `entt`, using the information 
/// in `desc`, and `enter_func`, and `exit_func`, to call later on collision events. 
///
/// @NOTE: The position, rotation, and user data of the given `desc` will be
/// set inside the function using the transform of `entt` and its ID respectively.
/// However, the rest of the memebers of `desc` must be filled by the caller.
///
/// Important to note that the type of the body MUST be either `PHYSICS_BODY_DYNAMIC` or `PHYSICS_BODY_KINEMATIC`.
FREYA_API DynamicBodyComponent& entity_add_dynamic_body(EntityWorld& world, 
                                                        EntityID& entt, 
                                                        PhysicsBodyDesc& desc, 
                                                        const OnCollisionFn& enter_func = nullptr, 
                                                        const OnCollisionFn& exit_func  = nullptr);

/// A helper function to add an animation component to `entt`, using the given 
/// `desc` and `tint`.
FREYA_API AnimationComponent& entity_add_animation(EntityWorld& world, 
                                                   EntityID& entt, 
                                                   const AnimationDesc& desc, 
                                                   const Vec4& tint = Vec4(1.0f));

/// A helper function to add an animator component to `entt`.
FREYA_API Animator& entity_add_animator(EntityWorld& world, EntityID& entt);

/// A helper function to call the on enter collision function for the given `entt` if it's available, 
/// passing in `other` and `normal`, and returning `true` if the function was successfully called, and `false` otherwise
FREYA_API bool entity_on_collision_enter(EntityWorld& world, EntityID& entt, EntityID& other, const Vec2& normal, void* user_data);

/// A helper function to call the on exit collision function for the given `entt` if it's available, 
/// passing in `other` and `normal`, and returning `true` if the function was successfully called, and `false` otherwise
FREYA_API bool entity_on_collision_exit(EntityWorld& world, EntityID& entt, EntityID& other, const Vec2& normal, void* user_data);

/// Retrieve a reference to a generic component `Comp` from `entt` that lives in the given `world`.
///
/// @NOTE: It is often advised to first use `entity_has_component` to first check 
/// if the given `entt` has that component in the first place.
template<typename Comp>
FREYA_API Comp& entity_get_component(EntityWorld& world, EntityID& entt) {
  return world.get<Comp>(entt);
}

/// Retrieve a const reference to a generic component `Comp` from `entt` that lives in the given `world`.
///
/// @NOTE: It is often advised to first use `entity_has_component` to first check 
/// if the given `entt` has that component in the first place.
template<typename Comp>
FREYA_API const Comp& entity_get_component_const(const EntityWorld& world, const EntityID& entt) {
  return world.get<Comp>(entt);
}

/// Return true if the given `Comp` component type currently exists in 
/// the given `entt` that lives in `world`.

template<typename Comp>
FREYA_API bool entity_has_component(EntityWorld& world, EntityID& entt) {
  return world.any_of<Comp>(entt);
}

template<typename Comp>
FREYA_API bool entity_has_component(const EntityWorld& world, const EntityID& entt) {
  return world.any_of<Comp>(entt);
}

/// EntityID functions
/// ----------------------------------------------------------------------

} // End of freya

//////////////////////////////////////////////////////////////////////////

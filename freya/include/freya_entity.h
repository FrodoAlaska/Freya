#pragma once

#include "freya_render.h"
#include "freya_physics.h"
#include "freya_noise.h"

//////////////////////////////////////////////////////////////////////////

namespace freya { // Start of freya

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

struct Entity;

/// Called when the physics body of `entt` is collided with `other` in the physics world.
using OnCollisionFn = std::function<void(EntityWorld& world, Entity& entt, Entity& other)>;

/// Callbacks
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Entity
struct Entity {
  /// Constructors

  public:
    Entity() :_id(ENTITY_NULL) {}
    Entity(const EntityID id) : _id(id) {}

  /// Functions

  public:
    void invalidate() {_id = ENTITY_NULL;}

  /// Getters

  public:
    inline const EntityID& get_id() const {return _id;}
    inline const EntityID& get_id() {return _id;}

  /// Operators
    
  public: 
    //
    // Assignment operators
    // 
    
    inline Entity& operator=(const EntityID& other) {_id = other;}
    inline Entity& operator=(const EntityID&& other) {_id = other;}


    //
    // Eqality operators
    // 

    inline bool operator==(const EntityID& rhs) {return _id == rhs;}
    inline bool operator!=(const EntityID& rhs) {return _id != rhs;}
    
    inline bool operator==(const EntityID& rhs) const {return _id == rhs;}
    inline bool operator!=(const EntityID& rhs) const {return _id != rhs;}

  /// Public memebers

  public:
    OnCollisionFn enter_func = nullptr;
    OnCollisionFn exit_func  = nullptr;

  /// Private members

  private:
    EntityID _id;
};
/// Entity
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// SpriteComponent
struct SpriteComponent {
  /// The texture that will 
  /// be given to the render command.
  ///
  /// @NOTE: If this texture is left as a 
  /// `nullptr`, the default texture will be used.
  GfxTexture* texture = nullptr; 

  /// The color/tint that will be used 
  /// in the render command. 
  Vec4 color;
};
/// SpriteComponent
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// TileSpriteComponent
struct TileSpriteComponent {
  /// The texture atlas that will be used 
  /// to source from while rendering.
  GfxTexture* texture_atlas = nullptr; 

  /// The source rect that will be used 
  /// with the texture atlas while rendering. 
  Rect2D source_rect        = {};

  /// The color/tint that will be used 
  /// in the render command. 
  ///
  /// @NOTE: This is `COLOR_WHITE` by default.
  Vec4 color                = COLOR_WHITE;
};
/// TileSpriteComponent
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// StaticBodyComponent
struct StaticBodyComponent {
  /// The internal handle of the physics body, 
  /// to be used with `physics_body_*` functions.
  PhysicsBodyID body; 
};
/// StaticBodyComponent
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// DynamicBodyComponent
struct DynamicBodyComponent {
  /// The internal handle of the physics body, 
  /// to be used with `physics_body_*` functions.
  PhysicsBodyID body; 
};
/// DynamicBodyComponent
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// AnimatorComponent
struct AnimatorComponent {
  /// Animation itself that will be 
  /// updated and rendered.
  Animation animation; 

  /// The tint that will be used 
  /// in the render command. 
  Vec4 tint;
};
/// AnimatorComponent
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

/// Render all the components of `world` in a data-oriented manner.
///
/// @NOTE: This function _MUST_ be called only once per frame after calling 
/// `renderer_begin` and _BEFORE_ `renderer_end`.
FREYA_API void entity_world_render(const EntityWorld& world);

/// EntityWorld functions
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// EntityID functions

/// Create an return a new entity in the given `world`, with `position`, 
/// `scale`, and `rotation` as its transform properties.
FREYA_API Entity entity_create(EntityWorld& world,
                               const Vec2& position, 
                               const Vec2& scale  = Vec2(1.0f), 
                               const f32 rotation = 0.0f);

/// Destroy the given `entt` and remove it and its components from `world`.
FREYA_API void entity_destroy(EntityWorld& world, Entity& entt);

/// Add a generic component `Comp` with `Args` initialization arguments 
/// to the given `entt` in the respective `world`.
template<typename Comp, typename... Args>
FREYA_API Comp& entity_add_component(EntityWorld& world, Entity& entt, Args&&... args) {
  return world.emplace<Comp>(entt.get_id(), std::forward<Args>(args)...);
}

/// A helper function to add an audio source to `entt`, using the information 
/// in `desc`, and the `audio_buffer_id` as the main buffer.
///
/// @NOTE: The position of the given `desc` will be set inside the function
/// using the current position of `entt`.
/// However, the rest of the memebers of `desc` must be filled by the caller.
FREYA_API AudioSourceID& entity_add_audio_source(EntityWorld& world, 
                                                 Entity& entt, 
                                                 AudioSourceDesc& desc, 
                                                 const AssetID& audio_buffer_id);

/// A helper function to add a timer component to `entt`, using the given 
/// `max_time`, `one_shot`, and `active` parameters, mirroring the 
/// `timer_create` function.
FREYA_API Timer& entity_add_timer(EntityWorld& world, 
                                  Entity& entt, 
                                  const f32 max_time, 
                                  const bool one_shot, 
                                  const bool active = true);

/// A helper function to add an animation component to `entt`, using the given 
/// `desc` and `tint`.
FREYA_API AnimatorComponent& entity_add_animation(EntityWorld& world, Entity& entt, const AnimationDesc& desc, const Vec4& tint = Vec4(1.0f));

/// A helper function to add a sprite component to `entt`, using the given
/// `texture_id` and `color` to give to the render command.
FREYA_API SpriteComponent& entity_add_sprite(EntityWorld& world, Entity& entt, const AssetID& texture_id, const Vec4& color = Vec4(1.0f));

/// A helper function to add a tile sprite component to `entt`, using the given
/// `texture_id`, `source`, and `color` to give to the render command.
FREYA_API TileSpriteComponent& entity_add_tile_sprite(EntityWorld& world, 
                                                      Entity& entt, 
                                                      const AssetID& texture_id,
                                                      const Rect2D& source, 
                                                      const Vec4& color = Vec4(1.0f));

/// A helper function to add a particle emitter to `entt`, using the information 
/// in `desc`.
///
/// @NOTE: The position of the given `desc` will be set inside the function
/// using the current position of `entt`.
/// However, the rest of the memebers of `desc` must be filled by the caller.
FREYA_API ParticleEmitter& entity_add_particle_emitter(EntityWorld& world, Entity& entt, ParticleEmitterDesc& desc);

/// A helper function to add a static body to `entt`, using the information 
/// in `desc`, and `enter_func` and `exit_func` to call later on collision events. 
///
/// @NOTE: The position, rotation, type, and user data of the given `desc` will be
/// set inside the function using the transform of `entt` and its ID respectively.
/// The type, too, will be set as `PHYSICS_BODY_STATIC`.
/// However, the rest of the memebers of `desc` must be filled by the caller.
FREYA_API StaticBodyComponent& entity_add_static_body(EntityWorld& world, 
                                                      Entity& entt, 
                                                      PhysicsBodyDesc& desc, 
                                                      const OnCollisionFn& enter_func = nullptr, 
                                                      const OnCollisionFn& exit_func  = nullptr);

/// A helper function to add a dynamic body to `entt`, using the information 
/// in `desc`, and `enter_func` and `exit_func` to call later on collision events. 
///
/// @NOTE: The position, rotation, and user data of the given `desc` will be
/// set inside the function using the transform of `entt` and its ID respectively.
/// However, the rest of the memebers of `desc` must be filled by the caller.
FREYA_API DynamicBodyComponent& entity_add_dynamic_body(EntityWorld& world, 
                                                        Entity& entt, 
                                                        PhysicsBodyDesc& desc, 
                                                        const OnCollisionFn& enter_func = nullptr, 
                                                        const OnCollisionFn& exit_func  = nullptr);

/// Retrieve a reference to a generic component `Comp` from `entt` that lives in the given `world`.
///
/// @NOTE: It is often advised to first use `entity_has_component` to first check 
/// if the given `entt` has that component in the first place.
template<typename Comp>
FREYA_API Comp& entity_get_component(EntityWorld& world, Entity& entt) {
  return world.get<Comp>(entt.get_id());
}

/// Retrieve a const reference to a generic component `Comp` from `entt` that lives in the given `world`.
///
/// @NOTE: It is often advised to first use `entity_has_component` to first check 
/// if the given `entt` has that component in the first place.
template<typename Comp>
FREYA_API const Comp& entity_get_component_const(const EntityWorld& world, const Entity& entt) {
  return world.get<Comp>(entt.get_id());
}

/// Return true if the given `Comp` component type currently exists in 
/// the given `entt` that lives in `world`.
template<typename Comp>
FREYA_API bool entity_has_component(EntityWorld& world, Entity& entt) {
  return world.any_of<Comp>(entt.get_id());
}

/// EntityID functions
/// ----------------------------------------------------------------------

} // End of freya

//////////////////////////////////////////////////////////////////////////

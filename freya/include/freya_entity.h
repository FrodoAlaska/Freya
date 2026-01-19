#pragma once

#include "freya_render.h"

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

/// Called when the physics body of `entt` is collided with `other` in the physics world.
using OnCollisionEnterFn = std::function<void(EntityWorld& world, EntityID& entt, EntityID& other)>;

/// Callbacks
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// SpriteComponent
struct SpriteComponent {
  /// The ID of the asset to be rendered. 
  /// Must be a texture asset.
  AssetID texture_id; 

  /// The color/tint that will be used 
  /// in the render command. 
  Vec4 color;
};
/// SpriteComponent
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
FREYA_API EntityID entity_create(EntityWorld& world,
                                 const Vec2& position, 
                                 const Vec2& scale, 
                                 const f32 rotation = 0.0f);

/// Destroy the given `entt` and remove it and its components from `world`.
FREYA_API void entity_destroy(EntityWorld& world, EntityID& entt);

/// Add a generic component `Comp` with `Args` initialization arguments 
/// to the given `entt` in the respective `world`.
template<typename Comp, typename... Args>
FREYA_API void entity_add_component(EntityWorld& world, EntityID& entt, Args&&... args) {
  world.emplace<Comp>(entt, std::forward<Args>(args)...);
}

/// A helper function to add an audio source to `entt`, using the information 
/// in `desc`, and the `audio_buffer_id` as the main buffer.
///
/// @NOTE: The position of the given `desc` will be set inside the function
/// using the current position of `entt`.
/// However, the rest of the memebers of `desc` must be filled by the caller.
FREYA_API void entity_add_audio_source(EntityWorld& world, 
                                        EntityID& entt, 
                                        AudioSourceDesc& desc, 
                                        const AssetID& audio_buffer_id);

/// A helper function to add a timer component to `entt`, using the given 
/// `max_time`, `one_shot`, and `active` parameters, mirroring the 
/// `timer_create` function.
FREYA_API void entity_add_timer(EntityWorld& world, 
                                 EntityID& entt, 
                                 const f32 max_time, 
                                 const bool one_shot, 
                                 const bool active = true);

/// A helper function to add an animation component to `entt`, using the given 
/// `desc` and `tint`.
FREYA_API void entity_add_animation(EntityWorld& world, EntityID& entt, const AnimationDesc& desc, const Vec4& tint = Vec4(1.0f));

/// A helper function to add a sprite component to `entt`, using the given
/// `texture_id` and `color` to give to the render command.
FREYA_API void entity_add_sprite(EntityWorld& world, EntityID& entt, const AssetID& texture_id, const Vec4& color = Vec4(1.0f));

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

/// EntityID functions
/// ----------------------------------------------------------------------

} // End of freya

//////////////////////////////////////////////////////////////////////////

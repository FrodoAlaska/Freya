#include "freya_entity.h"
#include "freya_event.h"
#include "freya_logger.h"

//////////////////////////////////////////////////////////////////////////

namespace freya { // Start of freya

/// ----------------------------------------------------------------------
/// EntityWorld functions

void entity_world_clear(EntityWorld& world) {
  // Destroy each entity

  auto view = world.view<EntityID>();
  for(auto entt_id : view) {
    Entity entt = Entity(entt_id);
    entity_destroy(world, entt);
  }

  // Goodbye, cruel world!
  world.clear();
}

void entity_world_update(EntityWorld& world, const f32 delta_time) {
  FREYA_PROFILE_FUNCTION();
  
  // DynamicBody
  {
    FREYA_PROFILE_FUNCTION_NAMED("entity_world_update(DynamicBodyComponent)");

    auto view = world.view<DynamicBodyComponent, Transform>();
    for(auto entt : view) {
      DynamicBodyComponent& body = view.get<DynamicBodyComponent>(entt);
      Transform& transform       = view.get<Transform>(entt); 

      transform.position = physics_body_get_position(body.body);
      transform.rotation = physics_body_get_rotation(body.body);
    }
  }

  // Animators
  {
    FREYA_PROFILE_FUNCTION_NAMED("entity_world_update(AnimatorComponent)");

    auto view = world.view<AnimatorComponent>();
    for(auto entt : view) {
      AnimatorComponent& anim = view.get<AnimatorComponent>(entt);
      animation_update(anim.animation, delta_time);
    }
  }

  // Timers
  {
    FREYA_PROFILE_FUNCTION_NAMED("entity_world_update(Timer)");

    auto view = world.view<Timer>();
    for(auto entt : view) {
      Timer& timer = view.get<Timer>(entt);
      timer_update(timer, delta_time);
    }
  }

  // ParticleEmitters
  {
    FREYA_PROFILE_FUNCTION_NAMED("entity_world_update(ParticleEmitter)");

    auto view = world.view<ParticleEmitter>();
    for(auto entt : view) {
      ParticleEmitter& emitter = view.get<ParticleEmitter>(entt);
      particle_emitter_update(emitter, delta_time);
    }
  }
}

void entity_world_render(const EntityWorld& world) {
  FREYA_PROFILE_FUNCTION();
  
  // Tile sprites
  {
    FREYA_PROFILE_FUNCTION_NAMED("entity_world_render(TileSpriteComponent)");

    auto view = world.view<TileSpriteComponent, Transform>();
    for(auto entt : view) {
      const Transform& transform        = view.get<Transform>(entt);
      const TileSpriteComponent& sprite = view.get<TileSpriteComponent>(entt);

      // Setup the dest rect

      Rect2D dest = {
        .size     = sprite.source_rect.size * transform.scale,
        .position = transform.position,
      };

      // Render a regular quad
      renderer_queue_texture(sprite.texture_atlas, sprite.source_rect, dest, transform.rotation, sprite.color);
    }
  }

  // Sprites
  {
    FREYA_PROFILE_FUNCTION_NAMED("entity_world_render(SpriteComponent)");

    auto view = world.view<SpriteComponent, Transform>();
    for(auto entt : view) {
      const Transform& transform    = view.get<Transform>(entt);
      const SpriteComponent& sprite = view.get<SpriteComponent>(entt);

      // Render a texture (if it's a valid)

      if(sprite.texture) {
        renderer_queue_texture(sprite.texture, transform, sprite.color);
        continue;
      }

      // Render a regular quad
      renderer_queue_quad(transform, sprite.color);
    }
  }
  
  // Animators
  {
    FREYA_PROFILE_FUNCTION_NAMED("entity_world_render(AnimatorComponent)");

    auto view = world.view<AnimatorComponent, Transform>();
    for(auto entt : view) {
      const Transform& transform    = view.get<Transform>(entt);
      const AnimatorComponent& anim = view.get<AnimatorComponent>(entt);

      renderer_queue_animation(anim.animation, transform, anim.tint);
    }
  }

  // ParticleEmitters
  {
    FREYA_PROFILE_FUNCTION_NAMED("entity_world_render(ParticleEmitter)");

    auto view = world.view<ParticleEmitter>();
    for(auto entt : view) {
      const ParticleEmitter& emitter = view.get<ParticleEmitter>(entt);
      renderer_queue_particles(emitter);
    }
  }
}

/// EntityWorld functions
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// EntityID functions

Entity entity_create(EntityWorld& world,
                     const Vec2& position, 
                     const Vec2& scale,
                     const f32 rotation) {
  // Generate an entity ID
  EntityID entt_id = world.create();

  // Add a transform component  

  Transform transform; 
  transform.position = position; 
  transform.rotation = rotation; 
  transform.scale    = scale;

  world.emplace<Transform>(entt_id, transform);
 
  // Create the new entity
  Entity entt = Entity(entt_id);

  // Dispatch an event

  Event event = {
    .type = EVENT_ENTITY_ADDED, 
    .entt = entt,
  };
  event_dispatch(event);

  // Done!
  return entt;
}

void entity_destroy(EntityWorld& world, Entity& entt) {
  // Dispatch an event

  Event event = {
    .type = EVENT_ENTITY_DESTROYED, 
    .entt = entt,
  };
  event_dispatch(event);

  // Destroy any components that require it 

  if(entity_has_component<DynamicBodyComponent>(world, entt)) {
    DynamicBodyComponent& body = world.get<DynamicBodyComponent>(entt.get_id());
    physics_body_destroy(body.body);
  }

  if(entity_has_component<StaticBodyComponent>(world, entt)) {
    StaticBodyComponent& body = world.get<StaticBodyComponent>(entt.get_id());
    physics_body_destroy(body.body);
  }

  // Destroy the entity in the world
  
  world.destroy(entt.get_id()); 
  entt.invalidate();
}

AudioSourceID& entity_add_audio_source(EntityWorld& world, 
                                       Entity& entt, 
                                       AudioSourceDesc& desc, 
                                       const AssetID& audio_buffer_id) {
  Transform& transform = world.get<Transform>(entt.get_id());

  desc.position      = transform.position; 
  desc.buffers[0]    = asset_group_get_audio_buffer(audio_buffer_id);
  desc.buffers_count = 1;

  return world.emplace<AudioSourceID>(entt.get_id(), audio_source_create(desc));
}

Timer& entity_add_timer(EntityWorld& world, 
                        Entity& entt, 
                        const f32 max_time, 
                        const bool one_shot, 
                        const bool active) {
  Timer timer; 
  timer_create(timer, max_time, one_shot, active);

  return world.emplace<Timer>(entt.get_id(), timer);
}

AnimatorComponent& entity_add_animation(EntityWorld& world, Entity& entt, const AnimationDesc& desc, const Vec4& tint) {
  Animation anim; 
  animation_create(anim, desc);

  return world.emplace<AnimatorComponent>(entt.get_id(), anim, tint);
}

SpriteComponent& entity_add_sprite(EntityWorld& world, Entity& entt, const AssetID& texture_id, const Vec4& color) {
  GfxTexture* texture = (texture_id.get_id() != ASSET_ID_INVALID) ? asset_group_get_texture(texture_id) : nullptr;
  return world.emplace<SpriteComponent>(entt.get_id(), texture, color);
}

TileSpriteComponent& entity_add_tile_sprite(EntityWorld& world, 
                                            Entity& entt, 
                                            const AssetID& texture_id,
                                            const Rect2D& source, 
                                            const Vec4& color) {
  GfxTexture* texture = asset_group_get_texture(texture_id);
  return world.emplace<TileSpriteComponent>(entt.get_id(), texture, source, color);
}

ParticleEmitter& entity_add_particle_emitter(EntityWorld& world, Entity& entt, ParticleEmitterDesc& desc) {
  Transform& transform = world.get<Transform>(entt.get_id());
  desc.position        = transform.position;
  desc.scale           = transform.scale;

  ParticleEmitter emitter; 
  particle_emitter_create(emitter, desc);

  return world.emplace<ParticleEmitter>(entt.get_id(), emitter);
}

StaticBodyComponent& entity_add_static_body(EntityWorld& world, 
                                            Entity& entt, 
                                            PhysicsBodyDesc& desc, 
                                            const OnCollisionFn& enter_func, 
                                            const OnCollisionFn& exit_func) {
  Transform& transform = world.get<Transform>(entt.get_id());

  desc.type          = PHYSICS_BODY_STATIC;
  desc.position      = transform.position;
  desc.rotation      = transform.rotation;
  desc.user_data     = (uintptr)entt.get_id();
  PhysicsBodyID body = physics_body_create(desc);

  entt.enter_func = enter_func;
  entt.exit_func  = exit_func;

  return world.emplace<StaticBodyComponent>(entt.get_id(), body);
}

DynamicBodyComponent& entity_add_dynamic_body(EntityWorld& world, 
                                              Entity& entt, 
                                              PhysicsBodyDesc& desc, 
                                              const OnCollisionFn& enter_func, 
                                              const OnCollisionFn& exit_func) {
  Transform& transform = world.get<Transform>(entt.get_id());

  desc.position      = transform.position;
  desc.rotation      = transform.rotation;
  desc.user_data     = (uintptr)entt.get_id();
  PhysicsBodyID body = physics_body_create(desc);

  entt.enter_func = enter_func;
  entt.exit_func  = exit_func;

  return world.emplace<DynamicBodyComponent>(entt.get_id(), body);
}


/// EntityID functions
/// ----------------------------------------------------------------------

} // End of freya

//////////////////////////////////////////////////////////////////////////

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
  for(auto entt : view) {
    entity_destroy(world, entt);
  }

  // Goodbye, cruel world!
  world.clear();
}

void entity_world_update(EntityWorld& world, const f32 delta_time) {
  FREYA_PROFILE_FUNCTION();
  
  // PhysicsBody
  {
    FREYA_PROFILE_FUNCTION_NAMED("entity_world_update(PhysicsBodyID)");

    auto view = world.view<PhysicsComponent, Transform>();
    for(auto entt : view) {
      PhysicsComponent& body = view.get<PhysicsComponent>(entt);
      Transform& transform   = view.get<Transform>(entt); 

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

EntityID entity_create(EntityWorld& world,
                       const Vec2& position, 
                       const Vec2& scale,
                       const f32 rotation) {
  // Create a new entity
  EntityID entt = world.create();

  // Add a transform component  

  Transform transform; 
  transform.position = position; 
  transform.rotation = rotation; 
  transform.scale    = scale;

  world.emplace<Transform>(entt, transform);

  // Dispatch an event

  Event event = {
    .type    = EVENT_ENTITY_ADDED, 
    .entt_id = entt,
  };
  event_dispatch(event);

  // Done!
  return entt;
}

void entity_destroy(EntityWorld& world, EntityID& entt) {
  // Dispatch an event

  Event event = {
    .type    = EVENT_ENTITY_DESTROYED, 
    .entt_id = entt,
  };
  event_dispatch(event);

  // Destroy any components that require it 

  if(entity_has_component<PhysicsComponent>(world, entt)) {
    PhysicsComponent& body = world.get<PhysicsComponent>(entt);
    physics_body_destroy(body.body);
  }

  // Destroy the entity in the world
  world.destroy(entt); 
}

AudioSourceID& entity_add_audio_source(EntityWorld& world, 
                                       EntityID& entt, 
                                       AudioSourceDesc& desc, 
                                       const AssetID& audio_buffer_id) {
  Transform& transform = world.get<Transform>(entt);

  desc.position      = transform.position; 
  desc.buffers[0]    = asset_group_get_audio_buffer(audio_buffer_id);
  desc.buffers_count = 1;

  return world.emplace<AudioSourceID>(entt, audio_source_create(desc));
}

Timer& entity_add_timer(EntityWorld& world, 
                        EntityID& entt, 
                        const f32 max_time, 
                        const bool one_shot, 
                        const bool active) {
  Timer timer; 
  timer_create(timer, max_time, one_shot, active);

  return world.emplace<Timer>(entt, timer);
}

AnimatorComponent& entity_add_animation(EntityWorld& world, EntityID& entt, const AnimationDesc& desc, const Vec4& tint) {
  Animation anim; 
  animation_create(anim, desc);

  return world.emplace<AnimatorComponent>(entt, anim, tint);
}

SpriteComponent& entity_add_sprite(EntityWorld& world, EntityID& entt, const AssetID& texture_id, const Vec4& color) {
  GfxTexture* texture = (texture_id.get_id() != ASSET_ID_INVALID) ? asset_group_get_texture(texture_id) : nullptr;
  return world.emplace<SpriteComponent>(entt, texture, color);
}

TileSpriteComponent& entity_add_tile_sprite(EntityWorld& world, 
                                            EntityID& entt, 
                                            const AssetID& texture_id,
                                            const Rect2D& source, 
                                            const Vec4& color) {
  GfxTexture* texture = asset_group_get_texture(texture_id);
  return world.emplace<TileSpriteComponent>(entt, texture, source, color);
}

ParticleEmitter& entity_add_particle_emitter(EntityWorld& world, EntityID& entt, ParticleEmitterDesc& desc) {
  Transform& transform = world.get<Transform>(entt);
  desc.position        = transform.position;

  ParticleEmitter emitter; 
  particle_emitter_create(emitter, desc);

  return world.emplace<ParticleEmitter>(entt, emitter);
}

PhysicsComponent& entity_add_physics_body(EntityWorld& world, 
                                          EntityID& entt, 
                                          PhysicsBodyDesc& desc, 
                                          const OnCollisionFn& enter_func, 
                                          const OnCollisionFn& exit_func) {
  Transform& transform = world.get<Transform>(entt);

  desc.position      = transform.position;
  desc.rotation      = transform.rotation;
  desc.user_data     = (u64)entt;
  PhysicsBodyID body = physics_body_create(desc);

  return world.emplace<PhysicsComponent>(entt, body, enter_func, exit_func);
}

/// EntityID functions
/// ----------------------------------------------------------------------

} // End of freya

//////////////////////////////////////////////////////////////////////////

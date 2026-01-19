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
  transform_apply(transform);

  world.emplace<Transform>(entt, transform);

  // Dispatch an event

  Event event = {
    .type    = EVENT_ENTITY_ADDED, 
    .entt_id = (u32)entt,
  };
  event_dispatch(event);

  // Done!
  return entt;
}

void entity_destroy(EntityWorld& world, EntityID& entt) {
  // Dispatch an event

  Event event = {
    .type    = EVENT_ENTITY_DESTROYED, 
    .entt_id = (u32)entt,
  };
  event_dispatch(event);

  // Destroy any components that require it 
  // @TODO (Entity)

  // Destroy the entity in the world
  world.destroy(entt); 
}

void entity_add_audio_source(EntityWorld& world, 
                             EntityID& entt, 
                             AudioSourceDesc& desc, 
                             const AssetID& audio_buffer_id) {
  Transform& transform = world.get<Transform>(entt);

  desc.position      = transform.position; 
  desc.buffers[0]    = asset_group_get_audio_buffer(audio_buffer_id);
  desc.buffers_count = 1;

  world.emplace<AudioSourceID>(entt, audio_source_create(desc));
}

void entity_add_timer(EntityWorld& world, 
                      EntityID& entt, 
                      const f32 max_time, 
                      const bool one_shot, 
                      const bool active) {
  Timer timer; 
  timer_create(timer, max_time, one_shot, active);

  world.emplace<Timer>(entt, timer);
}

void entity_add_animation(EntityWorld& world, EntityID& entt, const AnimationDesc& desc, const Vec4& tint) {
  Animation anim; 
  animation_create(anim, desc);

  world.emplace<AnimatorComponent>(entt, anim, tint);
}

void entity_add_sprite(EntityWorld& world, EntityID& entt, const AssetID& texture_id, const Vec4& color) {
  world.emplace<SpriteComponent>(entt, asset_group_get_texture(texture_id), color);
}

void entity_add_particle_emitter(EntityWorld& world, EntityID& entt, ParticleEmitterDesc& desc) {
  Transform& transform = world.get<Transform>(entt);
  desc.position        = transform.position;

  ParticleEmitter emitter; 
  particle_emitter_create(emitter, desc);

  world.emplace<ParticleEmitter>(entt, emitter);
}

/// EntityID functions
/// ----------------------------------------------------------------------

} // End of freya

//////////////////////////////////////////////////////////////////////////

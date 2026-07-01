#include "freya_entity.h"
#include "freya_event.h"
#include "freya_logger.h"
#include "freya_tilemap.h"
#include "freya_render.h"

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

  // Animations
  {
    FREYA_PROFILE_FUNCTION_NAMED("entity_world_update(AnimationComponent)");

    auto view = world.view<AnimationComponent>();
    for(auto entt : view) {
      AnimationComponent& anim = view.get<AnimationComponent>(entt);
      animation_update(anim.animation, delta_time);
    }
  }

  // Animators
  {
    FREYA_PROFILE_FUNCTION_NAMED("entity_world_update(Animator)");

    auto view = world.view<Animator>();
    for(auto entt : view) {
      Animator& anim = view.get<Animator>(entt);
      animator_update(anim, delta_time);
    }
  }

  // Timers
  {
    FREYA_PROFILE_FUNCTION_NAMED("entity_world_update(TimerComponent)");

    auto view = world.view<TimerComponent>();
    for(auto entt : view) {
      TimerComponent& comp = view.get<TimerComponent>(entt);
      timer_update(comp.timer, delta_time);

      if(comp.timer.has_runout && comp.runout_func) {
        comp.runout_func(world, entt, comp.user_data);
      }
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

  // UIContext
  {
    FREYA_PROFILE_FUNCTION_NAMED("entity_world_update(UIContext)");

    auto view = world.view<UIContext*>();
    for(auto entt : view) {
      UIContext* ctx = view.get<UIContext*>(entt);
      ui_context_update(ctx);
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
  // Generate an entity ID
  EntityID entt_id = world.create();

  // Add a transform component  

  Transform transform; 
  transform.position = position; 
  transform.rotation = rotation; 
  transform.scale    = scale;

  world.emplace<Transform>(entt_id, transform);

  // Dispatch an event

  Event event = {
    .type = EVENT_ENTITY_ADDED, 
    .entt = entt_id,
  };
  event_dispatch(event);

  // Done!
  return entt_id;
}

void entity_destroy(EntityWorld& world, EntityID& entt) {
  // Dispatch an event

  Event event = {
    .type = EVENT_ENTITY_DESTROYED, 
    .entt = entt,
  };
  event_dispatch(event);

  // Destroy any components that require it 

  if(entity_has_component<DynamicBodyComponent>(world, entt)) {
    DynamicBodyComponent& body = entity_get_component<DynamicBodyComponent>(world, entt);
    physics_body_destroy(body.body);
  }

  if(entity_has_component<StaticBodyComponent>(world, entt)) {
    StaticBodyComponent& body = entity_get_component<StaticBodyComponent>(world, entt);
    physics_body_destroy(body.body);
  }
  
  if(entity_has_component<NoiseGenerator*>(world, entt)) {
    NoiseGenerator* gen = entity_get_component<NoiseGenerator*>(world, entt);
    noise_generator_destroy(gen);
  }
  
  if(entity_has_component<Animator>(world, entt)) {
    Animator& anim = entity_get_component<Animator>(world, entt);
    animator_clear(anim);
  }
  
  if(entity_has_component<UIContext*>(world, entt)) {
    UIContext* ctx = entity_get_component<UIContext*>(world, entt);
    ui_context_destroy(ctx);
  }

  // Destroy the entity in the world
  world.destroy(entt); 
}

Camera& entity_add_camera(EntityWorld& world, EntityID& entt, CameraDesc& desc) {
  Transform& transform = world.get<Transform>(entt);
  desc.position        = transform.position;

  Camera& camera = world.emplace<Camera>(entt); 
  camera_create(camera, desc);

  return camera;
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

NoiseGenerator* entity_add_noise_generator(EntityWorld& world, EntityID& entt, const NoiseGeneratorDesc& desc) {
  NoiseGenerator* gen = noise_generator_create(desc);
  return world.emplace<NoiseGenerator*>(entt, gen);
}

TileMap& entity_add_tilemap(EntityWorld& world, 
                            EntityID& entt, 
                            const Vec2& start_pos, 
                            const Vec2& tile_size, 
                            const IVec2& tiles_count) {
  TileMap& tilemap = world.emplace<TileMap>(entt); 
  tilemap_create(tilemap, &world, start_pos, tile_size, tiles_count);

  return tilemap;
}

UIContext* entity_add_ui_context(EntityWorld& world, EntityID& entt, const String& name, const IVec2& view_bounds) {
  UIContext* ctx = ui_context_create(name, view_bounds);
  return world.emplace<UIContext*>(entt, ctx);
}

UIText& entity_add_text(EntityWorld& world, EntityID& entt, UITextDesc& desc) {
  Transform& transform = world.get<Transform>(entt);
  UIText& text         = world.emplace<UIText>(entt);

  desc.offset = transform.position;
  ui_text_create(text, desc);

  return text;
}

ParticleEmitter& entity_add_particle_emitter(EntityWorld& world, EntityID& entt, const ParticleEmitterDesc& desc) {
  ParticleEmitter& emitter = world.emplace<ParticleEmitter>(entt); 
  particle_emitter_create(emitter, desc);
  
  return emitter;
}

ParticleEmitter& entity_add_particle_emitter(EntityWorld& world, EntityID& entt, const AssetID& config_id) {
  ParticleEmitter& emitter = world.emplace<ParticleEmitter>(entt); 
  particle_emitter_create(emitter, config_id);
  
  return emitter;
}

TagComponent& entity_add_tag(EntityWorld& world, EntityID& entt, const String& tag) {
  return world.emplace<TagComponent>(entt, tag);
}

TimerComponent& entity_add_timer(EntityWorld& world, 
                                 EntityID& entt, 
                                 const TimerDesc& desc, 
                                 const OnTimerRunoutFn& runout_func, 
                                 void* user_data) {
  Timer timer; 
  timer_create(timer, desc);

  return world.emplace<TimerComponent>(entt, timer, runout_func, user_data);
}

SpriteComponent& entity_add_sprite(EntityWorld& world, 
                                   EntityID& entt, 
                                   const AssetID& texture_id, 
                                   const Vec4& color, 
                                   const i32 layer) {
  Texture texture = {};
  if(texture_id.get_id() != ASSET_ID_INVALID) {
    texture = asset_group_get_texture(texture_id);
  }

  return world.emplace<SpriteComponent>(entt, texture, color, layer);
}

TileSpriteComponent& entity_add_tile_sprite(EntityWorld& world, 
                                            EntityID& entt, 
                                            const AssetID& texture_id,
                                            const Rect2D& source, 
                                            const Vec4& color, 
                                            const i32 layer) {
  Texture texture = asset_group_get_texture(texture_id);
  return world.emplace<TileSpriteComponent>(entt, texture, source, color, layer);
}

StaticBodyComponent& entity_add_static_body(EntityWorld& world, 
                                            EntityID& entt, 
                                            PhysicsBodyDesc& desc, 
                                            const OnCollisionFn& enter_func, 
                                            const OnCollisionFn& exit_func) { 
  Transform& transform = world.get<Transform>(entt);

  desc.type          = PHYSICS_BODY_STATIC;
  desc.position      = transform.position;
  desc.rotation      = transform.rotation;
  desc.user_data     = (uintptr)entt;
  PhysicsBodyID body = physics_body_create(desc);

  return world.emplace<StaticBodyComponent>(entt, body, enter_func, exit_func);
}

DynamicBodyComponent& entity_add_dynamic_body(EntityWorld& world, 
                                              EntityID& entt, 
                                              PhysicsBodyDesc& desc, 
                                              const OnCollisionFn& enter_func, 
                                              const OnCollisionFn& exit_func) {
  FREYA_DEBUG_ASSERT((desc.type != PHYSICS_BODY_STATIC), "Invalid type given to dynamic body component");

  Transform& transform = world.get<Transform>(entt);

  desc.position      = transform.position;
  desc.rotation      = transform.rotation;
  desc.user_data     = (uintptr)entt;
  PhysicsBodyID body = physics_body_create(desc);

  return world.emplace<DynamicBodyComponent>(entt, body, enter_func, exit_func);
}

AnimationComponent& entity_add_animation(EntityWorld& world, EntityID& entt, const AnimationDesc& desc, const Vec4& tint) {
  Animation anim; 
  animation_create(anim, desc);

  return world.emplace<AnimationComponent>(entt, anim, tint);
}

Animator& entity_add_animator(EntityWorld& world, EntityID& entt) {
  return world.emplace<Animator>(entt);
}

bool entity_on_collision_enter(EntityWorld& world, EntityID& entt, EntityID& other, const Vec2& normal, void* user_data) {
  OnCollisionFn coll_func = nullptr;

  // Retrieve the function from the correct component

  if(entity_has_component<StaticBodyComponent>(world, entt)) {
    coll_func = entity_get_component<StaticBodyComponent>(world, entt).enter_func;
  }
  else if(entity_has_component<DynamicBodyComponent>(world, entt)) {
    coll_func = entity_get_component<DynamicBodyComponent>(world, entt).enter_func;
  }

  // Call the function

  if(!coll_func) {
    return false;
  }

  // Done!
  
  coll_func(world, entt, other, normal, user_data);
  return true;
}

bool entity_on_collision_exit(EntityWorld& world, EntityID& entt, EntityID& other, const Vec2& normal, void* user_data) {
  OnCollisionFn coll_func = nullptr;

  // Retrieve the function from the correct component

  if(entity_has_component<StaticBodyComponent>(world, entt)) {
    coll_func = entity_get_component<StaticBodyComponent>(world, entt).exit_func;
  }
  else if(entity_has_component<DynamicBodyComponent>(world, entt)) {
    coll_func = entity_get_component<DynamicBodyComponent>(world, entt).exit_func;
  }

  // Call the function

  if(!coll_func) {
    return false;
  }

  // Done!
  
  coll_func(world, entt, other, normal, user_data);
  return true;
}

/// EntityID functions
/// ----------------------------------------------------------------------

} // End of freya

//////////////////////////////////////////////////////////////////////////

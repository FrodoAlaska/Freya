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
  
  world.destroy(entt.get_id()); 
  entt.invalidate();
}

Camera& entity_add_camera(EntityWorld& world, Entity& entt, CameraDesc& desc) {
  Transform& transform = world.get<Transform>(entt.get_id());
  desc.position        = transform.position;

  Camera& camera = world.emplace<Camera>(entt.get_id()); 
  camera_create(camera, desc);

  return camera;
}

TagComponent& entity_add_tag(EntityWorld& world, Entity& entt, const String& tag) {
  return world.emplace<TagComponent>(entt.get_id(), tag);
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

UIContext* entity_add_ui_context(EntityWorld& world, Entity& entt, const String& name, const IVec2& view_bounds) {
  UIContext* ctx = ui_context_create(name, view_bounds);
  return world.emplace<UIContext*>(entt.get_id(), ctx);
}

AnimationComponent& entity_add_animation(EntityWorld& world, Entity& entt, const AnimationDesc& desc, const Vec4& tint) {
  Animation anim; 
  animation_create(anim, desc);

  return world.emplace<AnimationComponent>(entt.get_id(), anim, tint);
}

Animator& entity_add_animator(EntityWorld& world, Entity& entt) {
  return world.emplace<Animator>(entt.get_id());
}

SpriteComponent& entity_add_sprite(EntityWorld& world, Entity& entt, const AssetID& texture_id, const Vec4& color) {
  Texture texture = {};
  if(texture_id.get_id() != ASSET_ID_INVALID) {
    texture = asset_group_get_texture(texture_id);
  }

  return world.emplace<SpriteComponent>(entt.get_id(), texture, color);
}

TileSpriteComponent& entity_add_tile_sprite(EntityWorld& world, 
                                            Entity& entt, 
                                            const AssetID& texture_id,
                                            const Rect2D& source, 
                                            const Vec4& color) {
  Texture texture = asset_group_get_texture(texture_id);
  return world.emplace<TileSpriteComponent>(entt.get_id(), texture, source, color);
}

ParticleEmitter& entity_add_particle_emitter(EntityWorld& world, Entity& entt, const ParticleEmitterDesc& desc) {
  ParticleEmitter& emitter = world.emplace<ParticleEmitter>(entt.get_id()); 
  particle_emitter_create(emitter, desc);
  
  return emitter;
}

ParticleEmitter& entity_add_particle_emitter(EntityWorld& world, Entity& entt, const AssetID& config_id) {
  ParticleEmitter& emitter = world.emplace<ParticleEmitter>(entt.get_id()); 
  particle_emitter_create(emitter, config_id);
  
  return emitter;
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

  return world.emplace<StaticBodyComponent>(entt.get_id(), body, enter_func, exit_func);
}

DynamicBodyComponent& entity_add_dynamic_body(EntityWorld& world, 
                                              Entity& entt, 
                                              PhysicsBodyDesc& desc, 
                                              const OnCollisionFn& enter_func, 
                                              const OnCollisionFn& exit_func) {
  FREYA_DEBUG_ASSERT((desc.type != PHYSICS_BODY_STATIC), "Invalid type given to dynamic body component");

  Transform& transform = world.get<Transform>(entt.get_id());

  desc.position      = transform.position;
  desc.rotation      = transform.rotation;
  desc.user_data     = (uintptr)entt.get_id();
  PhysicsBodyID body = physics_body_create(desc);

  return world.emplace<DynamicBodyComponent>(entt.get_id(), body, enter_func, exit_func);
}

NoiseGenerator* entity_add_noise_generator(EntityWorld& world, Entity& entt, const NoiseGeneratorDesc& desc) {
  NoiseGenerator* gen = noise_generator_create(desc);
  return world.emplace<NoiseGenerator*>(entt.get_id(), gen);
}

TileMap& entity_add_tilemap(EntityWorld& world, 
                            Entity& entt, 
                            const Vec2& start_pos, 
                            const Vec2& tile_size, 
                            const IVec2& tiles_count) {
  TileMap& tilemap = world.emplace<TileMap>(entt.get_id()); 
  tilemap_create(tilemap, &world, start_pos, tile_size, tiles_count);

  return tilemap;
}

bool entity_on_collision_enter(EntityWorld& world, Entity& entt, Entity& other, const Vec2& normal, void* user_data) {
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

bool entity_on_collision_exit(EntityWorld& world, Entity& entt, Entity& other, const Vec2& normal, void* user_data) {
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

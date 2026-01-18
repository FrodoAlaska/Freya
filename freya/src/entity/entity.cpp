#include "freya_entity.h"
#include "freya_event.h"
#include "freya_logger.h"

//////////////////////////////////////////////////////////////////////////

namespace freya { // Start of freya

/// ----------------------------------------------------------------------
/// EntityWorld functions

void entity_world_clear(EntityWorld& world) {
  world.clear();
}

EntityID entity_world_create_entity(EntityWorld& world,
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

void entity_world_destroy_entity(EntityWorld& world, EntityID& entt) {
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

void entity_world_update(EntityWorld& world, const f32 delta_time) {
  FREYA_PROFILE_FUNCTION();

  // Timers
  {
    FREYA_PROFILE_FUNCTION_NAMED("entity_world_update(Timer)");

    auto view = world.view<Timer>();
    for(auto entt : view) {
      Timer& timer = view.get<Timer>(entt);
      timer_update(timer, delta_time);
    }
  }
}

void entity_world_render(const EntityWorld& world) {
  FREYA_PROFILE_FUNCTION();

  // Renderables
  {
    FREYA_PROFILE_FUNCTION_NAMED("entity_world_render(RenderableComponent)");

    auto view = world.view<RenderableComponent, Transform>();
    for(auto entt : view) {
      const Transform& transform            = view.get<Transform>(entt);
      const RenderableComponent& renderable = view.get<RenderableComponent>(entt);

      // Render a texture (if it's a valid)

      if(renderable.renderable_id.get_id() != ASSET_ID_INVALID) {
        renderer_queue_texture(renderable.renderable_id, transform, renderable.color);
        continue;
      }

      // Render a regular quad
      renderer_queue_quad(transform, renderable.color);
    }
  }
}

/// EntityWorld functions
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// EntityID functions

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
  timer_create(&timer, max_time, one_shot, active);

  world.emplace<Timer>(entt, timer);
}

void entity_add_renderable(EntityWorld& world, EntityID& entt, const AssetID& renderable_id, const Vec4& color) {
  world.emplace<RenderableComponent>(entt, renderable_id, color);
}

/// EntityID functions
/// ----------------------------------------------------------------------

} // End of freya

//////////////////////////////////////////////////////////////////////////

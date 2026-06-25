#include "app.h"

#include <freya.h>
#include <imgui.h>

/// ----------------------------------------------------------------------
/// App
struct App {
  freya::Window* window;
  freya::AssetGroupID group_id;

  freya::EntityWorld ecs; 
  freya::EntityID entt;
};

static App s_app;
/// App
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Callbacks

static bool on_asset_group_load(const freya::Event& event, const void* dispatcher, const void* listener) {
  freya::ParticleEmitter& emitter = freya::entity_get_component<freya::ParticleEmitter>(s_app.ecs, s_app.entt);
  freya::particle_emitter_create(emitter, freya::asset_group_get_id(s_app.group_id, "particles"));

  return true;
}

/// Callbacks
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// App functions 

bool app_init(const freya::Args& args, freya::Window* window) {
  // Window init
  s_app.window = window;

  // Editor init
  freya::gui_init(window);

  // Physics world init
  freya::physics_world_set_gravity(freya::Vec2(0.0f));

  // Assets init
  
  s_app.group_id = freya::asset_group_create("app_assets");

  freya::asset_group_build(s_app.group_id, "../../assets/assets_list.lua", "assets.frpkg");
  freya::asset_group_load_package(s_app.group_id, "assets.frpkg");
  
  // Renderer init
  
  freya::renderer_set_clear_color(freya::Vec4(0.1f, 0.1f, 0.1f, 1.0f));
  
  freya::renderer_apply_asset_group(s_app.group_id);
  freya::renderer_sumbit_world(&s_app.ecs);
  
  // Camera init
 
  freya::EntityID camera_entt = freya::entity_create(s_app.ecs, freya::Vec2(0.0f));

  freya::CameraDesc cam_desc = {
    .view_bounds = freya::window_get_size(s_app.window), 
    .zoom        = 1.0f,
  };
  freya::entity_add_camera(s_app.ecs, camera_entt, cam_desc);

  // Entity init
  s_app.entt = freya::entity_create(s_app.ecs, freya::Vec2(100.0f));
 
  // Particles init
  freya::entity_add_particle_emitter(s_app.ecs, s_app.entt, freya::asset_group_get_id(s_app.group_id, "particles"));

  // Listen to events
  freya::event_register(freya::EVENT_ASSET_GROUP_LOADED, on_asset_group_load);

  // Done!
  return true;
}

void app_shutdown() {
  freya::asset_group_destroy(s_app.group_id);
  freya::gui_shutdown();
}

void app_update(freya::f32 dt) {
  // Quit the application when the specified exit key is pressed
  
  if(freya::input_key_pressed(freya::KEY_ESCAPE)) {
    freya::event_dispatch(freya::Event{.type = freya::EVENT_APP_QUIT});
    return;
  }

  // Enable/disable debug mode

  if(freya::input_key_pressed(freya::KEY_F1)) {
    freya::gui_toggle_active();
    freya::physics_world_toggle_debug();
  }

  // Emit particles

  if(freya::input_key_pressed(freya::KEY_SPACE)) {
    freya::ParticleEmitter& emitter = freya::entity_get_component<freya::ParticleEmitter>(s_app.ecs, s_app.entt);
    freya::Transform& transform     = freya::entity_get_component<freya::Transform>(s_app.ecs, s_app.entt);

    freya::particle_emitter_emit(emitter, transform.position);
  }

  // Update
  freya::entity_world_update(s_app.ecs, dt);
}

void app_render_gui() {
  if(!freya::gui_is_active()) {
    return;
  }

  freya::gui_begin(); 
 
  // Debug
  freya::gui_debug_info();

  // Editor
   
  freya::gui_begin_panel("Editor");
  freya::gui_edit_entity_world("World", s_app.ecs);
  freya::gui_end_panel();

  freya::gui_end();
}

/// App functions 
/// ----------------------------------------------------------------------

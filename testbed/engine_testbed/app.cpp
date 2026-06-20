#include "app.h"

#include <freya.h>
#include <imgui.h>

/// ----------------------------------------------------------------------
/// App
struct App {
  freya::Window* window;
  freya::AssetGroupID group_id;

  freya::EntityWorld world; 
};

static App s_app;
/// App
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// App functions 

bool app_init(const freya::Args& args, freya::Window* window) {
  // Renderer init
  
  freya::renderer_set_clear_color(freya::Vec4(0.1f, 0.1f, 0.1f, 1.0f));
  freya::renderer_sumbit_world(&s_app.world);

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

  // Camera init 

  freya::Entity cam_entt = freya::entity_create(s_app.world, freya::Vec2(0.0f));
  
  freya::CameraDesc cam_desc = {
    .view_bounds = freya::window_get_size(s_app.window), 
    .zoom        = 1.0f,
  };
  freya::entity_add_camera(s_app.world, cam_entt, cam_desc);

  // Entity init

  freya::Entity entt = freya::entity_create(s_app.world, freya::Vec2(32.0f), freya::Vec2(32.0f));
  freya::entity_add_sprite(s_app.world, entt, {}, freya::COLOR_WHITE);

  freya::Vec2 pos = freya::Vec2(0.0f);
  for(freya::i32 i = 0; i < 10; i++) {
    freya::Entity entt = freya::entity_create(s_app.world, pos, freya::Vec2(32.0f));
    freya::entity_add_sprite(s_app.world, entt, freya::asset_group_get_id(s_app.group_id, "grass"));

    pos.x += 32.0f;
  }

  // Done!
  return true;
}

void app_shutdown() {
  freya::entity_world_clear(s_app.world);
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
  }

  // Update
  freya::entity_world_update(s_app.world, dt);
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
  freya::gui_edit_entity_world("Main", s_app.world);
  freya::gui_end_panel();

  freya::gui_end();
}

/// App functions 
/// ----------------------------------------------------------------------

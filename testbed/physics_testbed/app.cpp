#include "app.h"

#include <freya.h>
#include <imgui.h>

/// ----------------------------------------------------------------------
/// App
struct App {
  freya::Window* window;
  freya::Camera camera;
  freya::AssetGroupID group_id;

  freya::EntityWorld ecs;
  freya::Entity entt1;
};

static App s_app;
/// App
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// App functions 

bool app_init(const freya::Args& args, freya::Window* window) {
  // App init
  freya::renderer_set_clear_color(freya::Vec4(0.1f, 0.1f, 0.1f, 1.0f));

  // Window init
  s_app.window = window;

  // Editor init
  freya::gui_init(window);
  
  // Physics world init
  freya::physics_world_set_gravity(freya::Vec2(0.0f));

  // Camera init
  
  freya::CameraDesc cam_desc = {
    .position    = freya::Vec2(0.0f),
    .view_bounds = freya::window_get_size(s_app.window), 
    .zoom        = 1.0f,
  };
  freya::camera_create(s_app.camera, cam_desc);

  // Assets init
  s_app.group_id = freya::asset_group_create("app_assets");
 
  // Entity1 init

  s_app.entt1 = freya::entity_create(s_app.ecs, freya::Vec2(100.0f), freya::Vec2(128.0f));
  freya::entity_add_sprite(s_app.ecs, s_app.entt1, {}, freya::COLOR_WHITE);

  freya::PhysicsBodyDesc body_desc = {
    .type = freya::PHYSICS_BODY_DYNAMIC,
  };

  freya::DynamicBodyComponent& body = freya::entity_add_dynamic_body(s_app.ecs, s_app.entt1, body_desc);
  freya::collider_create(body.body, freya::ColliderDesc{}, freya::Vec2(0.0f), 32.0f);

  // Entity2 init

  freya::Entity entt2 = freya::entity_create(s_app.ecs, freya::Vec2(200.0f), freya::Vec2(64.0f));
  freya::entity_add_sprite(s_app.ecs, entt2, {}, freya::COLOR_RED);

  // Done!
  return true;
}

void app_shutdown() {
  freya::entity_world_clear(s_app.ecs);
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

  // Update the ECS 
  freya::entity_world_update(s_app.ecs, dt);

  // Move the camera
  freya::camera_move_top_down(s_app.camera, freya::Vec2(150.0f), dt);
}

void app_render() {
  // 2D render

  freya::renderer_begin(s_app.camera);

  // Draw the ECS
  freya::entity_world_render(s_app.ecs);
 
  // Draw the physics world

  if(freya::gui_is_active()) {
    freya::physics_world_draw_debug();
  }

  freya::renderer_end();

  // UI render
  
  // freya::ui_renderer_begin();
  // freya::ui_renderer_end();
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
  freya::gui_edit_camera("Camera", &s_app.camera);
  freya::gui_end_panel();

  freya::gui_end();
}

/// App functions 
/// ----------------------------------------------------------------------

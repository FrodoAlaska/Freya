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
  }

  // Update
  freya::entity_world_update(s_app.ecs, dt);
}

void app_render() {
  // 2D render

  freya::renderer_begin(s_app.camera);

  freya::Transform transform = {
    .position = freya::Vec2(10.0f),
    .scale    = freya::Vec2(32.0f),
  };
  freya::renderer_queue_quad(transform, freya::COLOR_WHITE);

  freya::renderer_end();
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

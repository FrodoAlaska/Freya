#include "app.h"

#include <freya.h>
#include <imgui.h>

/// ----------------------------------------------------------------------
/// App
struct App {
  freya::Window* window;
  freya::Camera camera;
  freya::AssetGroupID group_id;

  freya::PoissonDiskDesc poisson_desc;
  freya::DynamicArray<freya::Vec2> poisson_points;
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

  // Camera init
  
  freya::CameraDesc cam_desc = {
    .position = freya::Vec2(-90.0f, -40.0f),
    .zoom     = 10.6f,
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

  // Calculate the poisson disk

  if(!freya::gui_is_active() && freya::input_button_pressed(freya::MOUSE_BUTTON_LEFT)) {
    freya::poisson_disk_calculate(s_app.poisson_desc, s_app.poisson_points);
  }
}

void app_render() {
  // 2D render

  freya::renderer_begin(s_app.camera);

  // Render the points

  for(auto& point : s_app.poisson_points) {
    freya::Transform transform = {
      .position = point, 
      .scale    = freya::Vec2(0.3),
    };
    freya::renderer_queue_quad(transform, freya::COLOR_WHITE);
  }

  freya::renderer_end();

  // UI render
  
  freya::ui_renderer_begin();
  freya::ui_renderer_end();
}

void app_render_gui() {
  freya::gui_begin(); 
 
  // Debug
  freya::gui_debug_info();

  // Editor

  freya::gui_begin_panel("Editor");
 
  freya::gui_edit_camera("Camera", &s_app.camera);
  freya::gui_edit_poisson_disk("Poisson", &s_app.poisson_desc);
  
  freya::gui_end_panel();

  freya::gui_end();
}

/// App functions 
/// ----------------------------------------------------------------------

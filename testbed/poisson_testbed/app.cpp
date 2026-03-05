#include "app.h"

#include <freya.h>
#include <imgui/imgui.h>

/// ----------------------------------------------------------------------
/// App
struct freya::App {
  freya::Window* window;
  freya::Camera camera;
  freya::AssetGroupID group_id;

  freya::PoissonDiskDesc poisson_desc;
  freya::DynamicArray<freya::Vec2> poisson_points;
};
/// App
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// App functions 

freya::App* app_init(const freya::Args& args, freya::Window* window) {
  // App init
  
  freya::App* app = new freya::App{};
  freya::renderer_set_clear_color(freya::Vec4(0.1f, 0.1f, 0.1f, 1.0f));

  // Window init
  app->window = window;

  // Editor init
  freya::gui_init(window);

  // Camera init
  
  freya::CameraDesc cam_desc = {
    .position = freya::Vec2(-90.0f, -40.0f),
    .zoom     = 10.6f,
  };
  freya::camera_create(app->camera, cam_desc);

  // Assets init
  app->group_id = freya::asset_group_create("app_assets");

  // Done!
  return app;
}

void app_shutdown(freya::App* app) {
  freya::asset_group_destroy(app->group_id);
  freya::gui_shutdown();

  delete app;
}

void app_update(freya::App* app, const freya::f32 delta_time) {
  // Quit the application when the specified exit key is pressed
  
  if(freya::input_key_pressed(freya::KEY_ESCAPE)) {
    freya::event_dispatch(freya::Event{.type = freya::EVENT_APP_QUIT});
    return;
  }

  // Calculate the poisson disk

  if(!freya::gui_is_active() && freya::input_button_pressed(freya::MOUSE_BUTTON_LEFT)) {
    freya::poisson_disk_calculate(app->poisson_desc, app->poisson_points);
  }
}

void app_render(freya::App* app) {
  // 2D render

  freya::renderer_begin(app->camera);

  // Render the points

  for(auto& point : app->poisson_points) {
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

void app_render_gui(freya::App* app) {
  freya::gui_begin(); 
 
  // Debug
  freya::gui_debug_info();

  // Editor

  freya::gui_begin_panel("Editor");
 
  freya::gui_edit_camera("Camera", &app->camera);
  freya::gui_edit_poisson_disk("Poisson", &app->poisson_desc);
  
  freya::gui_end_panel();

  freya::gui_end();
}

/// App functions 
/// ----------------------------------------------------------------------

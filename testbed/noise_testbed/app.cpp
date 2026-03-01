#include "app.h"

#include <freya.h>
#include <imgui/imgui.h>

/// ----------------------------------------------------------------------
/// App
struct freya::App {
  freya::Window* window;
  freya::Camera camera;
  freya::AssetGroupID group_id;

  freya::NoiseGenerator* generator;
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
    .position = freya::Vec2(0.0f),
    .zoom     = 1.0f,
  };
  freya::camera_create(app->camera, cam_desc);

  // Assets init

  app->group_id = freya::asset_group_create("app_assets");
  
  freya::asset_group_build(app->group_id, "../../assets/asset_list.frlist", "assets.frpkg");
  freya::asset_group_load_package(app->group_id, "assets.frpkg");

  // Noise generator init
  app->generator = freya::noise_generator_create(freya::NoiseGeneratorDesc{});

  // Done!
  return app;
}

void app_shutdown(freya::App* app) {
  freya::noise_generator_destroy(app->generator);
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

  // Disable/enable the GUI
  
  if(freya::input_key_pressed(freya::KEY_F1)) {
    freya::gui_toggle_active();
    freya::physics_world_toggle_debug();
  }
}

void app_render(freya::App* app) {
  // 2D render

  freya::renderer_begin(app->camera);
  freya::renderer_end();

  // UI render
  
  freya::ui_renderer_begin();
  freya::ui_renderer_end();
}

void app_render_gui(freya::App* app) {
  if(!freya::gui_is_active()) {
    return;
  }

  freya::gui_begin(); 
 
  // Debug
  freya::gui_debug_info();

  // Editor

  freya::gui_begin_panel("Editor");

  freya::gui_edit_camera("Camera", &app->camera);
  freya::gui_edit_noise_generator("Nosie generator", app->generator);
  
  freya::gui_end_panel();

  freya::gui_end();
}

/// App functions 
/// ----------------------------------------------------------------------

#include "app.h"

#include <freya.h>

/// ----------------------------------------------------------------------
/// App
struct freya::App {
  freya::Window* window;
  freya::Camera camera;

  freya::AssetGroupID group_id;
};
/// App
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Private functions 

/// Private functions 
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
  };
  freya::camera_create(app->camera, cam_desc);

  // Assets init

  app->group_id = freya::asset_group_create("app_assets", "../../assets/asset_list.frlist", "assets.frpkg");
  
  freya::asset_group_build(app->group_id);
  freya::asset_group_load_package(app->group_id);

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

  // Disable/enable the GUI
  
  if(freya::input_key_pressed(freya::KEY_F1)) {
    freya::gui_toggle_active();
  }

  // Update the camera
  freya::camera_update(app->camera);
}

void app_render(freya::App* app) {
  freya::renderer_begin(app->camera);
  freya::renderer_end();
}

void app_render_gui(freya::App* app) {
  if(!freya::gui_is_active()) {
    return;
  }

  freya::gui_begin();
  freya::gui_end();
}

/// App functions 
/// ----------------------------------------------------------------------

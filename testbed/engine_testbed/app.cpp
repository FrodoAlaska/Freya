#include "app.h"

#include <freya.h>

/// ----------------------------------------------------------------------
/// App
struct freya::App {
  freya::Window* window;
  freya::Camera camera;

  freya::AssetGroupID group_id;

  freya::UIContext* context;
  freya::UIDocument* document;
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
  };
  freya::camera_create(app->camera, cam_desc);

  // Assets init

  app->group_id = freya::asset_group_create("app_assets");
  
  freya::asset_group_build(app->group_id, "../../assets/asset_list.frlist", "assets.frpkg");
  freya::asset_group_load_package(app->group_id, "assets.frpkg");

  // UI asset init

  freya::ui_renderer_set_asset_group(app->group_id);
  freya::ui_renderer_set_font("HeavyDataNerdFont");

  // UI init

  app->context  = freya::ui_context_create("Menu", freya::window_get_size(window));
  app->document = freya::ui_document_load(app->context, "ui/main_menu.rml");

  freya::ui_document_show(app->document);

  // Done!
  return app;
}

void app_shutdown(freya::App* app) {
  freya::ui_context_destroy(app->context);
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

  // Update the UI context
  freya::ui_context_update(app->context); 

  // Update the camera
  freya::camera_update(app->camera);
}

void app_render(freya::App* app) {
  // 2D render

  freya::Transform transform;
  transform.position = freya::Vec2(100.0f);
  transform.scale    = freya::Vec2(32.0f);

  freya::renderer_begin(app->camera);
  freya::renderer_queue_texture(freya::asset_group_get_id(app->group_id, "grass"), transform);
  freya::renderer_end();

  // UI render
  
  freya::ui_renderer_begin();
  freya::ui_context_render(app->context);
  freya::ui_renderer_end();
}

void app_render_gui(freya::App* app) {
  if(!freya::gui_is_active()) {
    return;
  }

  freya::gui_begin(); 
  freya::gui_debug_info();
  freya::gui_end();
}

/// App functions 
/// ----------------------------------------------------------------------

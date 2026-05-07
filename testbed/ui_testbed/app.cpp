#include "app.h"

#include <freya.h>
#include <imgui.h>

/// ----------------------------------------------------------------------
/// App
struct freya::App {
  freya::Window* window;
  freya::Camera camera;
  freya::AssetGroupID group_id;

  freya::UIContext* ui_ctx; 
  freya::UIDocument* ui_doc;
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
    .position    = freya::Vec2(0.0f),
    .view_bounds = freya::window_get_size(app->window), 
    .zoom        = 1.0f,
  };
  freya::camera_create(app->camera, cam_desc);

  // Assets init
  
  app->group_id = freya::asset_group_create("app_assets");

#if !defined(FREYA_DISTRIBUTE)
  freya::asset_group_build(app->group_id, "../../assets/assets_list.frlist", "assets.frpkg");
#endif

  freya::asset_group_load_package(app->group_id, "assets.frpkg");

  // UI renderer init

  freya::ui_renderer_set_asset_group(app->group_id);
  freya::ui_renderer_set_font("HeavyDataNerdFont");

  // UI context init
  app->ui_ctx = freya::ui_context_create("main", cam_desc.view_bounds);

  // UI document load

  const freya::AssetID& cfg_id  = freya::asset_group_get_id(app->group_id, "main_menu");
  const freya::UIConfig& ui_cfg = freya::asset_group_get_ui_config(cfg_id);

  app->ui_doc = freya::ui_document_load_from_memory(app->ui_ctx, ui_cfg);
  freya::ui_document_show(app->ui_doc);

  // Done!
  return app;
}

void app_shutdown(freya::App* app) {
  freya::ui_context_destroy(app->ui_ctx);
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

  // Enable/disable debug mode

  if(freya::input_key_pressed(freya::KEY_F1)) {
    freya::gui_toggle_active();
  }

  // Context update
  freya::ui_context_update(app->ui_ctx);
}

void app_render(freya::App* app) {
  // 2D render

  freya::renderer_begin(app->camera);
  freya::renderer_end();

  // UI render
  
  freya::ui_renderer_begin();
  freya::ui_context_render(app->ui_ctx);
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

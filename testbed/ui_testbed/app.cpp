#include "app.h"

#include <freya.h>
#include <imgui.h>

/// ----------------------------------------------------------------------
/// App
struct App {
  freya::Window* window;
  freya::Camera camera;
  freya::AssetGroupID group_id;

  freya::UIContext* ui_ctx; 
  freya::UIDocument* ui_doc;
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
    .position    = freya::Vec2(0.0f),
    .view_bounds = freya::window_get_size(s_app.window), 
    .zoom        = 1.0f,
  };
  freya::camera_create(s_app.camera, cam_desc);

  // Assets init
  
  s_app.group_id = freya::asset_group_create("app_assets");

#if !defined(FREYA_DISTRIBUTE)
  freya::asset_group_build(s_app.group_id, "../../assets/assets_list.lua", "assets.frpkg");
#endif

  freya::asset_group_load_package(s_app.group_id, "assets.frpkg");

  // UI renderer init

  freya::ui_renderer_set_asset_group(s_app.group_id);
  freya::ui_renderer_set_font("HeavyDataNerdFont");

  // UI context init
  s_app.ui_ctx = freya::ui_context_create("main", cam_desc.view_bounds);

  // UI document load

  const freya::AssetID& cfg_id  = freya::asset_group_get_id(s_app.group_id, "main_menu");
  const freya::UIConfig& ui_cfg = freya::asset_group_get_ui_config(cfg_id);

  s_app.ui_doc = freya::ui_document_load_from_memory(s_app.ui_ctx, ui_cfg);
  freya::ui_document_show(s_app.ui_doc);

  // Done!
  return true;
}

void app_shutdown() {
  freya::ui_context_destroy(s_app.ui_ctx);
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

  // Context update
  freya::ui_context_update(s_app.ui_ctx);
}

void app_render() {
  // 2D render

  freya::renderer_begin(s_app.camera);
  freya::renderer_end();

  // UI render
  freya::ui_renderer_apply_context(s_app.ui_ctx);
}

void app_render_gui() {
  if(!freya::gui_is_active()) {
    return;
  }

  freya::gui_begin(); 
  freya::gui_debug_info();
  freya::gui_end();
}

/// App functions 
/// ----------------------------------------------------------------------

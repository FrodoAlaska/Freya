#include "app.h"

#include <freya.h>
#include <imgui.h>

/// ----------------------------------------------------------------------
/// App
struct App {
  freya::Window* window;
  freya::AssetGroupID group_id;

  freya::EntityWorld world;
  freya::EntityID entt;
};

static App s_app;
/// App
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// App functions 

bool app_init(const freya::Args& args, freya::Window* window) {
  // Window init
  s_app.window = window;

  // Editor init
  freya::gui_init(window);

  // Assets init
  
  s_app.group_id = freya::asset_group_create("app_assets");

#if !defined(FREYA_DISTRIBUTE)
  freya::asset_group_build(s_app.group_id, "../../assets/assets_list.lua", "assets.frpkg");
#endif

  freya::asset_group_load_package(s_app.group_id, "assets.frpkg");
  
  // Renderer init
  
  freya::renderer_set_clear_color(freya::Vec4(0.1f, 0.1f, 0.1f, 1.0f));
  freya::renderer_sumbit_world(&s_app.world);
  
  freya::renderer_apply_asset_group(s_app.group_id);

  // Camera entity init
 
  freya::EntityID cam_entt = freya::entity_create(s_app.world, freya::Vec2(0.0f));

  freya::CameraDesc cam_desc = {
    .view_bounds = freya::window_get_size(s_app.window), 
    .zoom        = 1.0f,
  };
  freya::entity_add_camera(s_app.world, cam_entt, cam_desc);
  
  // UI button init

  s_app.entt = freya::entity_create(s_app.world, freya::Vec2(0.0f), freya::Vec2(96.0f, 64.0f));

  freya::UIButtonDesc button_desc = {
    .texture_id = {},

    .string = "Quit",

    .font_id   = freya::asset_group_get_id(s_app.group_id, "IosevkaNerdFont-Bold"),
    .font_size = 32.0f,

    .anchor  = freya::UI_ANCHOR_CENTER,

    .canvas_bounds = freya::window_get_size(s_app.window),
    .padding       = freya::Vec2(10.0f),
  };
  freya::entity_add_ui_button(s_app.world, s_app.entt, button_desc);

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

  // Check if the button is pressed

  freya::UIButton& button = freya::entity_get_component<freya::UIButton>(s_app.world, s_app.entt);
  if(freya::ui_button_pressed(button)) {
    freya::event_dispatch(freya::Event{.type = freya::EVENT_APP_QUIT});
    return;
  }

  // Update the world 
  freya::entity_world_update(s_app.world, dt);
}

void app_render_gui() {
  if(!freya::gui_is_active()) {
    return;
  }

  freya::gui_begin(); 
  freya::gui_debug_info();
  freya::gui_edit_entity_world("World", s_app.world);
  freya::gui_end();
}

/// App functions 
/// ----------------------------------------------------------------------

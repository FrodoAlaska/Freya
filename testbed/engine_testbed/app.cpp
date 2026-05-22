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
  freya::Entity entt;
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

  freya::asset_group_build(s_app.group_id, "../../assets/assets_list.lua", "assets.frpkg");
  freya::asset_group_load_package(s_app.group_id, "assets.frpkg");

  // Entity init
  s_app.entt = freya::entity_create(s_app.ecs, freya::Vec2(100.0f));
 
  // Animator init

  freya::Animator& anim = freya::entity_add_animator(s_app.ecs, s_app.entt);
  
  freya::animator_push_animation(anim, "idle", freya::Vec2(128.0f), freya::asset_group_get_id(s_app.group_id, "soldier_idle"));
  freya::animator_push_animation(anim, "walk", freya::Vec2(128.0f), freya::asset_group_get_id(s_app.group_id, "soldier_walk"));
  freya::animator_push_animation(anim, "run", freya::Vec2(128.0f), freya::asset_group_get_id(s_app.group_id, "soldier_run"));
  freya::animator_push_animation(anim, "attack", freya::Vec2(128.0f), freya::asset_group_get_id(s_app.group_id, "soldier_attack"));

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
    freya::physics_world_toggle_debug();
  }

  // Change animator state

  freya::Animator& animator = freya::entity_get_component<freya::Animator>(s_app.ecs, s_app.entt);

  if(freya::input_key_down(freya::KEY_LEFT_SHIFT) && freya::input_key_down(freya::KEY_D)) {
    freya::animator_switch(animator, "run");
  }
  else if(freya::input_key_down(freya::KEY_D)) {
    freya::animator_switch(animator, "walk");
  }
  else if(freya::input_key_down(freya::KEY_SPACE)) {
    freya::animator_switch(animator, "attack");
  }
  else {
    freya::animator_switch(animator, "idle");
  }

  // Update
  freya::entity_world_update(s_app.ecs, dt);
}

void app_render() {
  // 2D render

  freya::renderer_begin(s_app.camera);
  freya::entity_world_render(s_app.ecs);
  freya::renderer_end();

  // UI render
  
  freya::ui_renderer_begin();
  freya::ui_renderer_end();
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
  freya::gui_edit_entity("Entity", s_app.ecs, s_app.entt);
  freya::gui_end_panel();

  freya::gui_end();
}

/// App functions 
/// ----------------------------------------------------------------------

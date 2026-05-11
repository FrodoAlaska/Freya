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
  freya::Entity entt1, entt2;
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
 
  // Entity 1 init

  s_app.entt1 = freya::entity_create(s_app.ecs, freya::Vec2(0.0f), freya::Vec2(32.0f));

  freya::PhysicsBodyDesc body1_desc = {};
  freya::StaticBodyComponent& body1 = freya::entity_add_static_body(s_app.ecs, s_app.entt1, body1_desc);

  freya::DynamicArray<freya::Vec2> points = {
    freya::Vec2(-1.0f), 
    freya::Vec2(1.0f, -1.0f),
    freya::Vec2(0.0f, 1.0f),
  };
  freya::collider_create(body1.body, freya::ColliderDesc{}, points, 32.0f);

  freya::entity_add_sprite(s_app.ecs, s_app.entt1, freya::AssetID{}, freya::COLOR_GREEN);

  // Entity 2 init

  s_app.entt2 = freya::entity_create(s_app.ecs, freya::Vec2(48.0f, 0.0f), freya::Vec2(32.0f));

  freya::PhysicsBodyDesc body2_desc = {
    .type = freya::PHYSICS_BODY_DYNAMIC,

    .rotation_fixed = true,
  };

  freya::DynamicBodyComponent& body2 = freya::entity_add_dynamic_body(s_app.ecs, s_app.entt2, body2_desc);
  freya::collider_create(body2.body, freya::ColliderDesc{}, freya::Vec2(32.0f, 64.0f));

  freya::entity_add_sprite(s_app.ecs, s_app.entt2, freya::AssetID{}, freya::COLOR_WHITE);

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

  // Move entt2 
  
  freya::DynamicBodyComponent& body = freya::entity_get_component<freya::DynamicBodyComponent>(s_app.ecs, s_app.entt2);
  freya::Vec2 player_pos            = freya::physics_body_get_position(body.body); 

  static freya::i32 current_target = 0;
  freya::Vec2 targets[]            = {
    player_pos,
    freya::Vec2(128.0f, 0.0f),
    freya::Vec2(256.0f, 0.0f),
    freya::Vec2(512.0f, 0.0f),
  };

  if(freya::input_key_pressed(freya::KEY_LEFT)) {
    current_target--;
  }
  else if(freya::input_key_pressed(freya::KEY_RIGHT)) {
    current_target++;
  }

  current_target = freya::clamp_int(current_target, 0, 3);

  freya::physics_body_set_target_transform(body.body, targets[current_target], 0.0f, 120.0f * dt);

  freya::Vec2 center_screen = (freya::Vec2)s_app.camera.view_bounds / 2.0f;
  freya::camera_follow(s_app.camera, player_pos, -center_screen);

  // Ray test

  if(freya::input_key_pressed(freya::KEY_SPACE)) {
    freya::RayCastDesc ray_desc = {
      .origin    = player_pos, 
      .direction = freya::Vec2(1.0f, 0.0f),
      .distance  = 1000000.0f,
    };

    freya::CastResult result;
    if(freya::physics_world_cast_ray_closest(ray_desc, result)) {
      FREYA_LOG_TRACE("HIT!!");
    }
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
  freya::gui_edit_entity("Entity 1", s_app.ecs, s_app.entt1);
  freya::gui_edit_entity("Entity 2", s_app.ecs, s_app.entt2);

  freya::gui_end_panel();

  freya::gui_end();
}

/// App functions 
/// ----------------------------------------------------------------------

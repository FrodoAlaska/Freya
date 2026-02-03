#include "app.h"

#include <freya.h>
#include <imgui/imgui.h>

/// ----------------------------------------------------------------------
/// App
struct freya::App {
  freya::Window* window;
  freya::Camera camera;

  freya::AssetGroupID group_id;

  freya::EntityWorld world;
  freya::EntityID player_entt, ground_entt;
};
/// App
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Private functions

static bool on_body_hit(const freya::Event& event, const void* dispatcher, const void* listener) {
  freya::App* app = (freya::App*)listener; 

  freya::EntityID entt1 = (freya::EntityID)freya::physics_body_get_user_data(event.collision_data.body1);
  freya::EntityID entt2 = (freya::EntityID)freya::physics_body_get_user_data(event.collision_data.body2);
 
  freya::SpriteComponent& sprite = freya::entity_get_component<freya::SpriteComponent>(app->world, entt1);
  sprite.color = freya::COLOR_RED;

  return true;
}

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
    .zoom     = 1.0f,
  };
  freya::camera_create(app->camera, cam_desc);

  // Assets init

  app->group_id = freya::asset_group_create("app_assets");
  
  freya::asset_group_build(app->group_id, "../../assets/asset_list.frlist", "assets.frpkg");
  freya::asset_group_load_package(app->group_id, "assets.frpkg");

  // UI asset init

  freya::ui_renderer_set_asset_group(app->group_id);
  freya::ui_renderer_set_font("HeavyDataNerdFont");

  // Player init

  app->player_entt = freya::entity_create(app->world, freya::Vec2(100.0f), freya::Vec2(1.0f));

  freya::AnimationDesc anim_desc = {
    .texture_id = freya::asset_group_get_id(app->group_id, "enemy_skeleton1_death"),
    .frame_size = freya::Vec2(32.0f),

    .flip_speed = 1.0f,
  };
  freya::entity_add_animation(app->world, app->player_entt, anim_desc);
  
  freya::PhysicsBodyDesc body_desc = {
    .type = freya::PHYSICS_BODY_DYNAMIC,
  };

  freya::PhysicsComponent& comp = freya::entity_add_physics_body(app->world, app->player_entt, body_desc);
  freya::collider_create(comp.body, freya::ColliderDesc{}, freya::Vec2(0.0f), 32.0f);

  // Ground init

  app->ground_entt = freya::entity_create(app->world, freya::Vec2(100.0f, 500.0f), freya::Vec2(1.0f));
  freya::entity_add_sprite(app->world, app->ground_entt, freya::asset_group_get_id(app->group_id, "grass"));
  
  body_desc = {
    .type = freya::PHYSICS_BODY_STATIC,
  };

  freya::PhysicsComponent& ground_comp = freya::entity_add_physics_body(app->world, app->ground_entt, body_desc);
  freya::collider_create(ground_comp.body, freya::ColliderDesc{}, freya::Vec2(128.0f, 64.0f));

  // Events listen
  freya::event_register(freya::EVENT_PHYSICS_CONTACT_ADDED, on_body_hit, app);

  // Done!
  return app;
}

void app_shutdown(freya::App* app) {
  freya::entity_world_clear(app->world);

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

  // Move the entity

  freya::Vec2 direction = freya::Vec2(0.0f);
  
  if(freya::input_key_down(freya::KEY_W)) {
    direction.y = -1.0f; 
  }
  else if(freya::input_key_down(freya::KEY_S)) {
    direction.y = 1.0f; 
  }
  
  if(freya::input_key_down(freya::KEY_D)) {
    direction.x = 1.0f;
  }
  else if(freya::input_key_down(freya::KEY_A)) {
    direction.x = -1.0f;
  }

  freya::PhysicsComponent& comp = freya::entity_get_component<freya::PhysicsComponent>(app->world, app->player_entt);
  freya::physics_body_set_linear_velocity(comp.body, direction * 640.0f);

  // Move the camera
  
  freya::Vec2 center_screen = (freya::Vec2)freya::window_get_size(app->window) / 2.0f;
  freya::camera_follow(app->camera, freya::physics_body_get_position(comp.body), -center_screen);

  // Entity world update
  freya::entity_world_update(app->world, delta_time);
}

void app_render(freya::App* app) {
  // 2D render

  freya::renderer_begin(app->camera);
  freya::entity_world_render(app->world);
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
  freya::gui_edit_entity("Player", app->world, app->player_entt);
  freya::gui_edit_entity("Ground", app->world, app->ground_entt);

  freya::gui_end_panel();

  freya::gui_end();
}

/// App functions 
/// ----------------------------------------------------------------------

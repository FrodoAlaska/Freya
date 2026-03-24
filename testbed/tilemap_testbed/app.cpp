#include "app.h"

#include <freya.h>
#include <imgui/imgui.h>

/// ----------------------------------------------------------------------
/// App
struct freya::App {
  freya::Window* window;
  freya::Camera camera;
  freya::AssetGroupID group_id;

  freya::EntityWorld ecs;
  freya::TileMap tilemap;
  freya::Entity current_tile = freya::ENTITY_NULL;
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
 
  // TileMap init
  
  freya::tilemap_create(app->tilemap, &app->ecs, freya::Vec2(0.0f), freya::Vec2(32.0f), freya::IVec2(32));  
  for(auto& tile : app->tilemap.tiles) {
    freya::Transform& transform = freya::entity_get_component<freya::Transform>(app->ecs, tile);
    transform.scale             = app->tilemap.tile_size;
  }

  // Done!
  return app;
}

void app_shutdown(freya::App* app) {
  freya::tilemap_destroy(app->tilemap);

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

  // Select a tile

  if(freya::input_button_pressed(freya::MOUSE_BUTTON_LEFT)) {
    freya::Vec2 world_pos = freya::camera_screen_to_world_space(app->camera, app->window, freya::input_mouse_position());
    app->current_tile     = freya::tilemap_get_at(app->tilemap, world_pos + (app->tilemap.tile_size / 2.0f));
  }

  // Place a tile
  
  if(freya::input_button_pressed(freya::MOUSE_BUTTON_RIGHT)) {
    freya::Vec2 world_pos = freya::camera_screen_to_world_space(app->camera, app->window, freya::input_mouse_position());
    freya::Entity tile    = freya::tilemap_get_at(app->tilemap, world_pos + (app->tilemap.tile_size / 2.0f));

    if(!freya::entity_has_component<freya::SpriteComponent>(app->ecs, tile)) {
      freya::entity_add_sprite(app->ecs, tile, freya::AssetID{}, freya::COLOR_GREEN);
      freya::tilemap_place_at(app->tilemap, world_pos, tile);
    }
  }

  // Update the ECS
  freya::entity_world_update(app->ecs, delta_time);

  // Move the camera
  freya::camera_move_top_down(app->camera, freya::Vec2(350.0f), delta_time);
}

void app_render(freya::App* app) {
  // 2D render

  freya::renderer_begin(app->camera);

  freya::entity_world_render(app->ecs);

  if(app->current_tile != freya::ENTITY_NULL) {
    freya::Transform transform = freya::entity_get_component<freya::Transform>(app->ecs, app->current_tile);
    freya::renderer_queue_quad(transform, freya::Color(1.0f, 1.0f, 1.0f, 0.3f));
  }

  freya::Vec2 map_size       = app->tilemap.tile_size * (freya::Vec2)app->tilemap.tiles_count;
  freya::Transform transform = {
    .position = freya::Vec2(-16.0f) + (map_size / 2.0f),
    .scale    = map_size
  };
  freya::renderer_queue_quad(transform, freya::Color(1.0f, 1.0f, 1.0f, 0.1f));

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

  freya::Vec2 world_pos = freya::camera_screen_to_world_space(app->camera, app->window, freya::input_mouse_position()) + (app->tilemap.tile_size / 2.0f);
  ImGui::Text("%0.3f, %0.3f", world_pos.x, world_pos.y);

  freya::gui_edit_camera("Camera", &app->camera);
  
  if(app->current_tile != freya::ENTITY_NULL) {
    freya::gui_edit_entity("Current tile", app->ecs, app->current_tile);
  }

  freya::gui_end_panel();
  freya::gui_end();
}

/// App functions 
/// ----------------------------------------------------------------------

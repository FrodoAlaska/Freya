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
  freya::TileMap tilemap;
  freya::Entity current_tile = freya::ENTITY_NULL;
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
    .position = freya::Vec2(0.0f),
    .zoom     = 1.0f,
  };
  freya::camera_create(s_app.camera, cam_desc);

  // Assets init
  s_app.group_id = freya::asset_group_create("app_assets");
 
  // TileMap init
  
  freya::tilemap_create(s_app.tilemap, &s_app.ecs, freya::Vec2(0.0f), freya::Vec2(32.0f), freya::IVec2(32));  
  freya::tilemap_push_layer(s_app.tilemap, "main");

  // Done!
  return true;
}

void app_shutdown() {
  freya::tilemap_destroy(s_app.tilemap);

  freya::asset_group_destroy(s_app.group_id);
  freya::gui_shutdown();
}

void app_update(freya::f32 dt) {
  // Quit the application when the specified exit key is pressed
  
  if(freya::input_key_pressed(freya::KEY_ESCAPE)) {
    freya::event_dispatch(freya::Event{.type = freya::EVENT_APP_QUIT});
    return;
  }

  // Select a tile

  if(freya::input_button_pressed(freya::MOUSE_BUTTON_LEFT)) {
    freya::Vec2 world_pos = freya::camera_screen_to_world_space(s_app.camera, freya::input_mouse_position());
    s_app.current_tile    = freya::tilemap_get_at(s_app.tilemap, world_pos + (s_app.tilemap.tile_size / 2.0f));
  }

  // Place a tile
  
  if(freya::input_button_pressed(freya::MOUSE_BUTTON_RIGHT)) {
    freya::Vec2 world_pos = freya::camera_screen_to_world_space(s_app.camera, freya::input_mouse_position());
    freya::Vec2 tile_pos  = world_pos + (s_app.tilemap.tile_size / 2.0f);

    freya::Entity& tile = freya::tilemap_get_at(s_app.tilemap, tile_pos);

    if(tile == freya::ENTITY_NULL) {
      tile = freya::tilemap_place_at(s_app.tilemap, tile_pos);
      
      freya::entity_get_component<freya::Transform>(s_app.ecs, tile).scale = s_app.tilemap.tile_size;
      freya::entity_add_sprite(s_app.ecs, tile, freya::AssetID{}, freya::COLOR_GREEN);
    }
  }

  // Update the ECS
  freya::entity_world_update(s_app.ecs, dt);

  // Move the camera
  freya::camera_move_top_down(s_app.camera, freya::Vec2(350.0f), dt);
}

void app_render() {
  // 2D render

  freya::renderer_begin(s_app.camera);

  freya::entity_world_render(s_app.ecs);

  if(s_app.current_tile != freya::ENTITY_NULL) {
    freya::Transform& transform = freya::entity_get_component<freya::Transform>(s_app.ecs, s_app.current_tile);
    transform.scale             = s_app.tilemap.tile_size;

    freya::renderer_queue_quad(transform, freya::Color(1.0f, 1.0f, 1.0f, 0.3f));
  }

  freya::Vec2 map_size       = s_app.tilemap.tile_size * (freya::Vec2)s_app.tilemap.tiles_count;
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

void app_render_gui() {
  freya::gui_begin(); 
 
  // Debug
  freya::gui_debug_info();

  // Editor

  freya::gui_begin_panel("Editor");

  freya::Vec2 world_pos = freya::camera_screen_to_world_space(s_app.camera, freya::input_mouse_position()) + (s_app.tilemap.tile_size / 2.0f);
  ImGui::Text("%0.3f, %0.3f", world_pos.x, world_pos.y);

  freya::gui_edit_camera("Camera", &s_app.camera);
  
  if(s_app.current_tile != freya::ENTITY_NULL) {
    freya::gui_edit_entity("Current tile", s_app.ecs, s_app.current_tile);
  }

  freya::gui_end_panel();
  freya::gui_end();
}

/// App functions 
/// ----------------------------------------------------------------------

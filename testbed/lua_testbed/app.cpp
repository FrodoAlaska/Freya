#include "app.h"

#include <freya.h>
#include <imgui.h>

/// ----------------------------------------------------------------------
/// Spell
struct Spell {
  freya::String name = "INVALID";
  freya::String desc = "INVALID";

  freya::i32 mana_cost = 0;
  freya::i32 power     = 0;

  freya::i32 range  = 0; 
  freya::i32 radius = 0;
};
/// Spell
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// App
struct App {
  freya::Window* window;
  freya::Camera camera;
  freya::AssetGroupID group_id;

  freya::EntityWorld ecs; 
  freya::DynamicArray<Spell> spells;
};

static App s_app;
/// App
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Private functions

static void read_config(const freya::AssetID& config_id) {
  // Get the LUA state
  
  s_app.spells.clear();
  lua_State* lua = freya::asset_group_get_lua_state(config_id);

  // Iterate through all the spells

  lua_getglobal(lua, "spells");
  freya::u32 spells_count = lua_rawlen(lua, -1);

  for(freya::u32 i = 1; i <= spells_count; i++) {
    // Push `spells[i]` onto the stack 
     
    Spell spell;
    lua_geti(lua, -1, i);

    // Get the name
     
    lua_getfield(lua, -1, "name");
    spell.name = lua_tostring(lua, -1);
    lua_pop(lua, 1);

    // Get the desc
     
    lua_getfield(lua, -1, "desc");
    spell.desc = lua_tostring(lua, -1);
    lua_pop(lua, 1);

    // Get the mana 
     
    lua_getfield(lua, -1, "mana_cost");
    spell.mana_cost = lua_tonumber(lua, -1);
    lua_pop(lua, 1);

    // Get the power
     
    lua_getfield(lua, -1, "power");
    spell.power = lua_tonumber(lua, -1);
    lua_pop(lua, 1);

    // Get the range
     
    lua_getfield(lua, -1, "range");
    spell.range = lua_tonumber(lua, -1);
    lua_pop(lua, 1);

    // Get the radius
     
    lua_getfield(lua, -1, "radius");
    spell.radius = lua_tonumber(lua, -1);
    lua_pop(lua, 1);

    // Add the new spell 
    s_app.spells.push_back(spell);

    lua_pop(lua, 1);
  }

  lua_pop(lua, 1);
}

/// Private functions
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Callbacks

static bool on_asset_reload(const freya::Event& event, const void* dispatcher, const void* listener) {
  read_config(freya::asset_group_get_id(s_app.group_id, "spells"));
  return true;
}

/// Callbacks
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

  // Config init
  read_config(freya::asset_group_get_id(s_app.group_id, "spells"));

  // Listen to events 
  freya::event_register(freya::EVENT_ASSET_GROUP_LOADED, on_asset_reload);

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
  for(auto& spell : s_app.spells) {
    ImGui::SeparatorText(spell.name.c_str());
    ImGui::Text("Description: %s", spell.desc.c_str());
    ImGui::Text("Mana: %i", spell.mana_cost);
    ImGui::Text("Power: %i", spell.power);
    ImGui::Text("Range: %i", spell.range);
    ImGui::Text("Radius: %i", spell.radius);
  }
  freya::gui_end_panel();

  freya::gui_end();
}

/// App functions 
/// ----------------------------------------------------------------------

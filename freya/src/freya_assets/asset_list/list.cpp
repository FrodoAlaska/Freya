#include "list.h"

#include "freya_logger.h"

/// ----------------------------------------------------------------------
/// Private functions

static void assign_section_paths(ListContext& list, const freya::String& asset_str, const freya::AssetType asset_type) {
  // Make sure that this type actually exists in the file 

  freya::i32 type = lua_getfield(list.lua_state, -1, asset_str.c_str());
  if(type == LUA_TNIL) {
    lua_pop(list.lua_state, 1);
    return;
  }

  // Assign a new section

  ListSection section = {}; 
  section.type        = asset_type; 

  // Assign the main directory of this section 

  lua_getfield(list.lua_state, -1, "directory");
  section.directory = lua_tostring(list.lua_state, -1);
  lua_pop(list.lua_state, 1);

  //
  // @TODO (Asset list): Add settings for extensions and indivisual items
  //

  // Iterate through the directory to add all of the paths

  freya::FilePath full_path = freya::filepath_append(list.parent_dir, section.directory);
  freya::filesystem_directory_recurse_iterate(full_path, [&](const freya::FilePath& base_dir, const freya::FilePath& current_path, void* user_data){
    section.assets.emplace_back(current_path);
    return true;
  });

  // Done!
  
  lua_pop(list.lua_state, 1);
  list.sections.push_back(section);
}

/// Private functions
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// ListContext functions 

bool list_context_load(ListContext& out_list, const freya::FilePath& path) {
  // Open the LUA file

  freya::File file;
  if(!freya::file_open(file, path, (freya::i32)freya::FILE_OPEN_READ)) {
    return false;
  }
  
  // Read the string from the file
 
  freya::String lua_source;

  freya::file_read_string(file, &lua_source);
  freya::file_close(file);
  
  // Initialize LUA

  out_list.lua_state = luaL_newstate();
  
  luaopen_base(out_list.lua_state);
  luaopen_table(out_list.lua_state);
 
  // Load the LUA file

  freya::i32 load_res = luaL_loadstring(out_list.lua_state, lua_source.c_str());
  if(load_res != LUA_OK) {
    FREYA_LOG_WARN("LUA-ERROR: %s", lua_tostring(out_list.lua_state, -1));
    lua_pop(out_list.lua_state, 1);

    return false;
  }
  
  // Run the whole LUA file

  freya::i32 run_res = lua_pcall(out_list.lua_state, 0, 0, 0);
  if(run_res != LUA_OK) {
    FREYA_LOG_WARN("LUA-ERROR: %s", lua_tostring(out_list.lua_state, -1));
    lua_pop(out_list.lua_state, 1);

    return false;
  }

  // Assing the parent directory of the list
  out_list.parent_dir = freya::filepath_parent_path(path);

  // 
  // Go through each asset type and add the paths 
  //

  freya::i32 type = lua_getglobal(out_list.lua_state, "sections");
  if(type == LUA_TNIL) {
    FREYA_LOG_ERROR("Failed to read \'sections\' type in LUA asset file at \'%s\'", path.c_str());
    return false;
  }

  assign_section_paths(out_list, "textures", freya::ASSET_TYPE_TEXTURE);
  assign_section_paths(out_list, "fonts", freya::ASSET_TYPE_FONT);
  assign_section_paths(out_list, "audio", freya::ASSET_TYPE_AUDIO_BUFFER);
  assign_section_paths(out_list, "lua", freya::ASSET_TYPE_LUA);

  lua_pop(out_list.lua_state, 1);

  // Done!
  return true;
}

void list_context_unload(ListContext& list) {
  for(auto& section : list.sections) {
    section.assets.clear();
  }
  list.sections.clear();

  lua_close(list.lua_state);
}

/// ListContext functions 
/// ----------------------------------------------------------------------

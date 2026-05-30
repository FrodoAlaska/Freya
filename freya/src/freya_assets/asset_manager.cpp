#include "freya_assets.h"
#include "freya_event.h"
#include "freya_logger.h"
#include "freya_memory.h"
#include "freya_render.h"

#include "asset_list/list.h"
#include "loaders/asset_loaders.h"

#include <cstring>

//////////////////////////////////////////////////////////////////////////

namespace freya { // Start of freya

/// ----------------------------------------------------------------------
/// AssetManager 
struct AssetManager {
  AssetGroupID cache_id;
  HashMap<i32, AssetGroup> groups;
};

static AssetManager s_manager;
/// AssetManager 
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Macros (Unfortunately)

#define PUSH_ASSET(group, assets, asset, type, asset_id) {            \
  group.assets.push_back(asset);                                      \
  asset_id = AssetID(type, group.id, (i16)(group.assets.size() - 1)); \
}

#define GROUP_CHECK(group_id) FREYA_ASSERT(group_id != ASSET_GROUP_INVALID)

/// Macros (Unfortunately)
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Private functions 

template<typename T> 
static T& get_asset(const AssetID& id, DynamicArray<T>& asset, const AssetType type) {
  FREYA_DEBUG_ASSERT((id == type), "Invalid type when trying to retrieve a resource");
  FREYA_DEBUG_ASSERT((id >= 0 && id <= (i16)asset.size()), "Invalid ID when trying to retrieve a resource");

  return asset[id.get_id()];
}

static bool can_build_frpkg(const FilePath& assets_path, const FilePath& output_path) {
  // Check if output exists... 
  // we need to build a package if it doesn't exist

  if(!filesystem_exists(output_path)) {
    return true; 
  }

  // We also need to build (or update) the package if it 
  // was modified recently (or more recently than the package file)

  bool has_changed          = false; 
  FileTimePoint output_time = filesystem_get_last_write_time(output_path);

  // Iterate through each directory and check its write time

  auto iterate_dir = [&](const FilePath& base_dir, const FilePath& current_path, void* user_data) {
    FileTimePoint current_time = filesystem_get_last_write_time(current_path);
    if(current_time > output_time) {
      has_changed = true;
      return false;
    }

    return true;
  };
  filesystem_directory_iterate(assets_path, iterate_dir);

  // No changes. No need to build

  if(has_changed) {
    return true;
  }

  // No need to build the package...
  return false;
}

static void build_textures(File& pkg_file, const ListSection& section) {
  FREYA_PROFILE_FUNCTION();

  // Write the number of assets of this type

  u16 asset_count = (u16)section.assets.size(); 
  file_write_bytes(pkg_file, &asset_count, sizeof(asset_count));

  // Load and write all of the assets

  for(const auto& path : section.assets) {
    // Write the name of the asset

    FilePath name = filepath_stem(path);
    file_write_bytes(pkg_file, name);

    // Load the asset

     
    sg_sampler_desc sampler_desc = {};
    sampler_desc.min_filter      = SG_FILTER_NEAREST;
    sampler_desc.mag_filter      = SG_FILTER_NEAREST;

    sg_image_desc image_desc = {};
    texture_loader_load(path, image_desc);
     
    // 
    // Write the asset 
    //
  
    // Write the texture's size

    u16 width  = (u16)image_desc.width;
    u16 height = (u16)image_desc.height;

    file_write_bytes(file, &width, sizeof(width));
    file_write_bytes(file, &height, sizeof(height));

    // Write the format

    u8 format = (u8)image_desc.pixel_format;
    file_write_bytes(file, &format, sizeof(format));

    // Write the filters 

    u8 min_filter = (u8)sampler_desc.min_filter;
    u8 mag_filter = (u8)sampler_desc.mag_filter;

    file_write_bytes(file, &min_filter, sizeof(min_filter));
    file_write_bytes(file, &mag_filter, sizeof(mag_filter));

    // Write the data

    u32 pixel_size = (format == SG_PIXELFORMAT_RGBA16F) ? 4 : 1; 
    u32 data_size  = (width * height) * 4 * pixel_size; // 4 = color components

    file_write_bytes(file, image_desc.data.mip_levels[0].ptr, data_size);

    // Unload the data
    texture_loader_unload(image_desc); 
  }
}

static void build_fonts(File& pkg_file, const ListSection& section) {
  FREYA_PROFILE_FUNCTION();

  // Write the number of assets of this type

  u16 asset_count = (u16)section.assets.size(); 
  file_write_bytes(pkg_file, &asset_count, sizeof(asset_count));
  
  // Load and write all of the assets
  
  for(const auto& path : section.assets) {
    // Write the name of the asset

    FilePath name = filepath_stem(path);
    file_write_bytes(pkg_file, name);

    // Load the asset

    DynamicArray<u8> bytes;
    font_loader_load(path, bytes);

    // Save the asset
    
    u32 data_size = (u32)bytes.size();

    file_write_bytes(pkg_file, &data_size, sizeof(data_size));
    file_write_bytes(pkg_file, bytes.data(), bytes.size());
  }
}

static void build_audio_buffers(File& pkg_file, const ListSection& section) {
  FREYA_PROFILE_FUNCTION();

  // Write the number of assets of this type

  u16 asset_count = (u16)section.assets.size(); 
  file_write_bytes(pkg_file, &asset_count, sizeof(asset_count));
  
  // Load and write all of the assets
  
  for(const auto& path : section.assets) {
    // Write the name of the asset

    FilePath name = filepath_stem(path);
    file_write_bytes(pkg_file, name);

    // Load the asset

    AudioBufferDesc audio_desc{};
    audio_loader_load(path, &audio_desc);

    //
    // Write the asset
    //
  
    // Write the format

    u8 format = (u8)audio_desc.format;
    file_write_bytes(file, &format, sizeof(format));

    // Write the channels

    u8 channels = (u8)audio_desc.channels;
    file_write_bytes(file, &channels, sizeof(channels));

    // Write the sample rate
    file_write_bytes(file, &audio_desc.sample_rate, sizeof(audio_desc.sample_rate));

    // Write the samples

    u32 size = (u32)audio_desc.size;

    file_write_bytes(file, &size, sizeof(size));
    file_write_bytes(file, audio_desc.data, size);
  }
}

static void build_ui_config(File& pkg_file, const ListSection& section) {
  FREYA_PROFILE_FUNCTION();

  // Write the number of assets of this type

  u16 asset_count = (u16)section.assets.size(); 
  file_write_bytes(pkg_file, &asset_count, sizeof(asset_count));
  
  // Load and write all of the assets
  
  for(const auto& path : section.assets) {
    // Write the name of the asset

    FilePath name = filepath_stem(path);
    file_write_bytes(pkg_file, name);

    // Load and save the asset

    UIConfig ui_cfg{};

    ui_config_loader_load(path, &ui_cfg);
    file_write_bytes(pkg_file, ui_cfg.html_source);
  }
}

static void build_lua_state(File& pkg_file, const ListSection& section) {
  FREYA_PROFILE_FUNCTION();

  // Write the number of assets of this type

  u16 asset_count = (u16)section.assets.size(); 
  file_write_bytes(pkg_file, &asset_count, sizeof(asset_count));
  
  // Load and write all of the assets
  
  for(const auto& path : section.assets) {
    // Write the name of the asset

    FilePath name = filepath_stem(path);
    file_write_bytes(pkg_file, name);

    // Load and save the asset

    String src;

    lua_state_loader_load(path, &src);
    file_write_bytes(pkg_file, src);
  }
}

static void read_textures(File& file, AssetGroup& group) {
  FREYA_PROFILE_FUNCTION();
  
  // Read the count

  u16 count;
  file_read_bytes(file, &count, sizeof(count));

  // Read the asset

  for(u16 i = 0; i < count; i++) {
    // Read the name

    String name;
    file_read_bytes(file, &name);

    //
    // Read the texture desc
    //

    sg_image_desc image_desc     = {};
    sg_sampler_desc sampler_desc = {};

    // Read the texture's size

    u16 width, height;
    file_read_bytes(file, &width, sizeof(width));  
    file_read_bytes(file, &height, sizeof(height));  

    image_desc.width  = width;
    image_desc.height = height;

    // Read the format

    u8 format;
    file_read_bytes(file, &format, sizeof(format));

    image_desc.pixel_format = (sg_pixel_format)format;

    // Read the filters

    u8 min_filter, mag_filter;

    file_read_bytes(file, &min_filter, sizeof(min_filter));
    file_read_bytes(file, &mag_filter, sizeof(mag_filter));

    sampler_desc.min_filter = (sg_filter)min_filter;
    sampler_desc.mag_filter = (sg_filter)mag_filter;

    // Read the data

    u32 pixel_size = (format == SG_PIXELFORMAT_RGBA16F) ? 4 : 1; 
    u32 data_size  = (width * height) * 4 * pixel_size; // 4 = color components

    image_desc.data.mip_levels[0].ptr  = memory_allocate(data_size);
    image_desc.data.mip_levels[0].size = data_size;

    file_read_bytes(file, image_desc.data.mip_levels[0].ptr, data_size);

    // Add the texture to the group
    group.named_ids[name] = asset_group_push_texture(group.id, image_desc, sampler_desc); 

    // Get rid of the texture data on the CPU-side
    memory_free(image_desc.data.mip_levels[0].ptr); 
    
    // Done!
    FREYA_LOG_DEBUG("Loaded texture \'%s\' from frpkg ", name.c_str());
  }
}

static void read_fonts(File& file, AssetGroup& group) {
  FREYA_PROFILE_FUNCTION();
  
  // Read the count

  u16 count;
  file_read_bytes(file, &count, sizeof(count));

  // Read the asset

  for(u16 i = 0; i < count; i++) {
    // Read the name

    String name;
    file_read_bytes(file, &name);

    // Read the font data
   
    u32 data_size;
    file_read_bytes(file, &data_size, sizeof(data_size));

    DynamicArray<u8> font_data;
    font_data.resize(data_size);
    
    file_read_bytes(file, font_data.data(), data_size);

    // Add the font to the group
    group.named_ids[name] = asset_group_push_font(group.id, font_data, name); 

    FREYA_LOG_DEBUG("Loaded font \'%s\' from frpkg ", name.c_str());
  }
}

static void read_audio_buffers(File& file, AssetGroup& group) {
  FREYA_PROFILE_FUNCTION();
  
  // Read the count

  u16 count;
  file_read_bytes(file, &count, sizeof(count));

  // Read the asset

  for(u16 i = 0; i < count; i++) {
    // Read the name

    String name;
    file_read_bytes(file, &name);
    
    //
    // Read the audio desc
    //

    AudioBufferDesc desc;
  
    // Read the format
 
    u8 format;
    file_read_bytes(file, &format, sizeof(format));

    out_desc->format = (AudioBufferFormat)format;

    // Read the channels

    u8 channels;
    file_read_bytes(file, &channels, sizeof(channels));

    out_desc->channels = (u32)channels;

    // Read the sample rate
    file_read_bytes(file, &out_desc->sample_rate, sizeof(out_desc->sample_rate));

    // Read the samples

    u32 size;
    file_read_bytes(file, &size, sizeof(size));

    out_desc->size = (sizei)size;
    out_desc->data = memory_allocate(out_desc->size);

    file_read_bytes(file, out_desc->data, out_desc->size);

    // Add the audio buffer to the group
    group.named_ids[name] = asset_group_push_audio_buffer(group.id, desc); 

    // Get rid of the audio data on the CPU-side
    memory_free(desc.data); 

    // Done!
    FREYA_LOG_DEBUG("Loaded audio buffer \'%s\' from frpkg ", name.c_str());
  }
}

static void read_ui_config(File& file, AssetGroup& group) {
  FREYA_PROFILE_FUNCTION();
  
  // Read the count

  u16 count;
  file_read_bytes(file, &count, sizeof(count));

  // Read the asset

  for(u16 i = 0; i < count; i++) {
    // Read the name

    String name;
    file_read_bytes(file, &name);

    // Read the ui config

    String html;
    file_read_bytes(file, &html);

    // Add the ui config to the group
    group.named_ids[name] = asset_group_push_ui_config(group.id, html); 

    FREYA_LOG_DEBUG("Loaded UI config \'%s\' from frpkg ", name.c_str());
  }
}

static void read_lua_state(File& file, AssetGroup& group) {
  FREYA_PROFILE_FUNCTION();
  
  // Read the count

  u16 count;
  file_read_bytes(file, &count, sizeof(count));

  // Read the asset

  for(u16 i = 0; i < count; i++) {
    // Read the name

    String name;
    file_read_bytes(file, &name);

    // Read the LUA file

    String src;
    file_read_bytes(file, &src);

    // Add the LUA state to the group
    group.named_ids[name] = asset_group_push_lua_state(group.id, src); 

    FREYA_LOG_DEBUG("Loaded LUA state \'%s\' from frpkg ", name.c_str());
  }
}

static bool build_package(const FilePath& list_path, const FilePath& output_path) {
  // Load the frlist file

  ListContext list_ctx;
  if(!list_context_load(list_ctx, list_path)) {
    FREYA_LOG_ERROR("Failed to read asset list file at \'%s\'", list_path.c_str());
    return false;
  }

  // Open the frpkg file to write to later
 
  File pkg_file;
  i32 file_flags = (i32)(FILE_OPEN_WRITE | FILE_OPEN_BINARY);

  if(!file_open(pkg_file, output_path, file_flags)) {
    FREYA_LOG_ERROR("Failed to open frpkg file at \'%s\'", output_path.c_str());
    return false;
  }

  // Write the version
  file_write_bytes(pkg_file, &FRPKG_VALID_VERSION, sizeof(FRPKG_VALID_VERSION));

  // Write the number of sections
  
  u8 sections_count = (u8)list_ctx.sections.size();
  file_write_bytes(pkg_file, &sections_count, sizeof(sections_count));

  // Convert each asset in the list and write it to the newly-create frpkg file

  FREYA_LOG_DEBUG("Converting assets from \'%s\' to \'%s\'", list_ctx.parent_dir.c_str(), output_path.c_str());

  for(auto& section : list_ctx.sections) {
    // Write the asset type
     
    u8 asset_type = (u8)section.type;
    file_write_bytes(pkg_file, &asset_type, sizeof(asset_type));

    // Write the actual assets

    switch(section.type) {
      case ASSET_TYPE_TEXTURE:
        build_textures(pkg_file, section);
        break;
      case ASSET_TYPE_SHADER:
        // @TODO (Assets/shaders): Do we even need to build shaders???
        break;
      case ASSET_TYPE_FONT:
        build_fonts(pkg_file, section);
        break;
      case ASSET_TYPE_AUDIO_BUFFER:
        build_audio_buffers(pkg_file, section);
        break;
      case ASSET_TYPE_UI_CONFIG:
        build_ui_config(pkg_file, section);
        break;
      case ASSET_TYPE_LUA:
        build_lua_state(pkg_file, section);
        break;
      default:
        break;
    }
  }

  // Done!
  
  file_close(pkg_file);
  list_context_unload(list_ctx);

  FREYA_LOG_DEBUG("Successfully built frpkg at \'%s\'!", output_path.c_str());

  return true;
}

/// Private functions 
/// ----------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Asset manager functions

void asset_manager_init() {
  // Create the cache asset group

  s_manager.cache_id               = AssetGroupID(ASSET_CACHE_ID);
  s_manager.groups[ASSET_CACHE_ID] = AssetGroup {
    .name = "cache", 
    .id   = s_manager.cache_id,
  };

  // Done!
  FREYA_LOG_INFO("Successfully created the global asset manager");
}

void asset_manager_shutdown() {
  // Shutdown the cache asset group
  asset_group_destroy(s_manager.cache_id); 

  // Done!
  FREYA_LOG_INFO("The global asset manager was successfully shutdown");
}

AssetGroup& asset_manager_get_group(const AssetGroupID& group_id) {
  return s_manager.groups[group_id.get_id()];
}

/// Asset manager functions
///---------------------------------------------------------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// AssetGroupID functions

AssetGroupID asset_group_create(const String& name) {
  // Group init

  AssetGroupID id               = AssetGroupID((i32)random_u32() + 1);
  s_manager.groups[id.get_id()] = AssetGroup {
    .name = name, 
    .id   = id,
  };

  // Done!

  FREYA_LOG_INFO("Successfully created an asset group \'%s\'", name.c_str());
  return id;
}

void asset_group_clear(const AssetGroupID& group_id) {
  GROUP_CHECK(group_id);
  AssetGroup& group = s_manager.groups[group_id.get_id()];

  //
  // Destroy GFX compound assets
  //

  for(auto& asset : group.shader_contexts) {
    delete asset;
  }
  group.shader_contexts.clear();
  
  for(auto& asset : group.fonts) {
    delete asset;
  }
  group.fonts.clear();

  //
  // Destroy GFX assets
  //

  for(auto& asset : group.buffers) {
    gfx_buffer_destroy(asset);
  }
  group.buffers.clear();

  for(auto& asset : group.textures) {
    gfx_texture_destroy(asset);
  }
  group.textures.clear();

  for(auto& asset : group.shaders) {
    gfx_shader_destroy(asset);
  }
  group.shaders.clear();

  // 
  // Destroy other assets
  //
  
  group.ui_configs.clear();

  for(auto& asset : group.lua_states) {
    lua_close(asset);
  }
  group.lua_states.clear();

  for(auto& asset : group.audio_buffers) {
    audio_buffer_destroy(asset);
  }
  group.audio_buffers.clear();

  // Destroy any other members
  group.named_ids.clear();

  // Done!
  FREYA_LOG_INFO("Asset group \'%s\' was successfully cleared", group.name.c_str());
}

void asset_group_destroy(const AssetGroupID& group_id) {
  // Clear the group
  asset_group_clear(group_id);

  // Destroy anything that survived the clear
  
  AssetGroup& group = s_manager.groups[group_id.get_id()];
  for(auto& watcher : group.watchers) {
    filewatcher_destroy(watcher);
  }
  
  // Done!

  FREYA_LOG_INFO("Asset group \'%s\' was successfully destroyed", group.name.c_str());
  s_manager.groups.erase(group_id.get_id());
}

void asset_group_reload(const AssetGroupID& group_id) {
  // Clear the group
  asset_group_clear(group_id);

  // Load the package again
  
  AssetGroup& group = s_manager.groups[group_id.get_id()];
  asset_group_load_package(group_id, group.frpkg_path);
}

bool asset_group_build(const AssetGroupID& group_id, const FilePath& list_path, const FilePath& output_path) {
  GROUP_CHECK(group_id);

  AssetGroup& group    = s_manager.groups[group_id.get_id()];
  FilePath assets_path = filepath_parent_path(list_path);

  // Create a watcher to watch the main asset files 
  // @TEMP

  FilePath paths[] = {
    "textures", 
    "shaders",
    "fonts",
    "audio", 
    "ui",
    "lua",
  };

  for(sizei i = 0; i < group.watchers.size(); i++) {
    if(!group.watchers[i]) {
      FilePath dir = filepath_append(assets_path, paths[i]);
      FREYA_LOG_DEBUG("Watching directory \'%s\'", dir.c_str());

      group.watchers[i] = filewatcher_create(dir, [=](const FilePath& path, const FileStatus status) {
        switch(status) {
          case FILE_STATUS_MODIFIED:
            build_package(list_path, output_path);
            asset_group_reload(group_id);
            break;
          default:
            break;
        }
      });
    }
  }

  // We need to check if it's even needed to build the package. 
  // For example, it might already be up-to-date.

  if(!can_build_frpkg(assets_path, output_path)) {
    FREYA_LOG_DEBUG("Frpkg at \'%s\' is up-to-date", output_path.c_str());
    return true; // No need to build the package... 
  }

  // Done!
  return build_package(list_path, output_path);
}

AssetID asset_group_push_buffer(const AssetGroupID& group_id, const sg_buffer_desc& buff_desc) {
  GROUP_CHECK(group_id);
  AssetGroup& group = s_manager.groups[group_id.get_id()];

  // Create and push the buffer

  sg_buffer buffer = sg_make_buffer(buff_desc); 

  AssetID id;
  PUSH_ASSET(group, buffers, buffer, ASSET_TYPE_BUFFER, id);

  // Some useful debug info
  
  FREYA_LOG_DEBUG("Group \'%s\' pushed buffer:", group.name.c_str());
  FREYA_LOG_DEBUG("     Size = %zu", buff_desc.data.size);

  // Done!
  return id;
}

AssetID asset_group_push_texture(const AssetGroupID& group_id, 
                                 const sg_image_desc& image_desc, 
                                 const sg_sampler_desc& sampler_desc) {
  GROUP_CHECK(group_id);
  AssetGroup& group = s_manager.groups[group_id.get_id()];
  
  // Create the texture
  
  Texture texture;

  texture.size  = Vec2(image_desc.width, image_desc.height);
  texture.image = sg_make_image(image_desc);

  sg_view_desc view_desc  = {};
  view_desc.texture.image = texture.image; 

  texture.view    = sg_make_view(view_desc);
  texture.sampler = sg_make_sampler(sampler_desc); 

  // Push the texture

  AssetID id; 
  PUSH_ASSET(group, textures, texture, ASSET_TYPE_TEXTURE, id);
 
  // Some useful debug info
  
  FREYA_LOG_DEBUG("Group \'%s\' pushed texture:", group.name.c_str());
  FREYA_LOG_DEBUG("     Size = %i X %i", image_desc.width, image_desc.height);
 
  // Done!
  return id;
}

AssetID asset_group_push_shader(const AssetGroupID& group_id, const sg_shader_desc& shader_desc) {
  GROUP_CHECK(group_id);
  AssetGroup& group = s_manager.groups[group_id.get_id()];
  
  // Create and load the shader
  sg_shader shader = sg_make_shader(shader_desc);

  // New shader ID

  AssetID id; 
  PUSH_ASSET(group, shaders, shader, ASSET_TYPE_SHADER, id);
  
  // Some useful debug info
  
  FREYA_LOG_DEBUG("Group \'%s\' pushed shader:", group.name.c_str());
  if(!shader_desc.compute_source.empty()) {
    FREYA_LOG_DEBUG("     Compute source length = %zu", strlen(shader_desc.compute_func.source));
  }
  else {
    FREYA_LOG_DEBUG("     Vertex source length = %zu", strlen(shader_desc.vertex_func.source));
    FREYA_LOG_DEBUG("     Pixel source length  = %zu", strlen(shader_desc.fragment_func.source));
  }

  // Done!
  return id;
}

AssetID asset_group_push_font(const AssetGroupID& group_id, const DynamicArray<u8>& font_data, const String& name) {
  GROUP_CHECK(group_id);
  AssetGroup& group = s_manager.groups[group_id.get_id()];
  
  // Allocate a new font
  Font* font = new Font{name, font_data};

  // New font added!
  
  AssetID id;
  PUSH_ASSET(group, fonts, font, ASSET_TYPE_FONT, id);

  // Some useful debug info
  
  FREYA_LOG_DEBUG("Group \'%s\' pushed a font:", group.name.c_str());
  FREYA_LOG_DEBUG("     Data size = %zu", font_data.size());

  // Done!
  return id;
}

AssetID asset_group_push_audio_buffer(const AssetGroupID& group_id, const AudioBufferDesc& audio_desc) {
  GROUP_CHECK(group_id);
  AssetGroup& group = s_manager.groups[group_id.get_id()];
  
  // Create a new audio buffer
  AudioBufferID buffer = audio_buffer_create(audio_desc);

  // New audio buffer added!
  
  AssetID id;
  PUSH_ASSET(group, audio_buffers, buffer, ASSET_TYPE_AUDIO_BUFFER, id);

  // Some useful debug info
  
  FREYA_LOG_DEBUG("Group \'%s\' pushed an audio buffer:", group.name.c_str());
  FREYA_LOG_DEBUG("     Format      = %s", audio_format_str(audio_desc.format));
  FREYA_LOG_DEBUG("     Channels    = %i", audio_desc.channels);
  FREYA_LOG_DEBUG("     Size        = %zu", audio_desc.size);
  FREYA_LOG_DEBUG("     Sample Rate = %zu", audio_desc.sample_rate);

  // Done!
  return id;
}

AssetID asset_group_push_ui_config(const AssetGroupID& group_id, const String& html_source) {
  GROUP_CHECK(group_id);
  AssetGroup& group = s_manager.groups[group_id.get_id()];
  
  // Create a new UI config
  UIConfig cfg = {html_source};

  // New audio buffer added!
  
  AssetID id;
  PUSH_ASSET(group, ui_configs, cfg, ASSET_TYPE_UI_CONFIG, id);

  // Some useful debug info
  
  FREYA_LOG_DEBUG("Group \'%s\' pushed a UI config:", group.name.c_str());
  FREYA_LOG_DEBUG("     Length = %zu", html_source.size());

  // Done!
  return id;
}

AssetID asset_group_push_lua_state(const AssetGroupID& group_id, const String& lua_source) {
  GROUP_CHECK(group_id);
  AssetGroup& group = s_manager.groups[group_id.get_id()];
 
  // @TEMP(LUA): We'll probably have a dedicated system to handle all of this 
  // instead of just leaving it inside the asset group.

  // Create a new LUA config
  
  lua_State* state = luaL_newstate();

  luaopen_base(state);
  luaopen_table(state);

  // Load the LUA file

  i32 load_res = luaL_loadstring(state, lua_source.c_str());
  if(load_res != LUA_OK) {
    FREYA_LOG_WARN("LUA-ERROR: %s", lua_tostring(state, -1));
    lua_pop(state, 1);

    return AssetID{};
  }

  // Run the whole LUA file

  i32 run_res = lua_pcall(state, 0, 0, 0);
  if(run_res != LUA_OK) {
    FREYA_LOG_WARN("LUA-ERROR: %s", lua_tostring(state, -1));
    lua_pop(state, 1);

    return AssetID{};
  }

  // New LUA state added!
  
  AssetID id;
  PUSH_ASSET(group, lua_states, state, ASSET_TYPE_LUA, id);

  // Some useful debug info
  
  FREYA_LOG_DEBUG("Group \'%s\' pushed a new LUA state:", group.name.c_str());
  FREYA_LOG_DEBUG("     Length = %zu", lua_source.size());

  // Done!
  return id;
}

bool asset_group_load_package(const AssetGroupID& group_id, const FilePath& frpkg_path) {
  GROUP_CHECK(group_id);
  AssetGroup& group = s_manager.groups[group_id.get_id()];

  // Open the frpkg file

  File file; 
  i32 file_flags = (i32)(FILE_OPEN_READ | FILE_OPEN_BINARY);

  if(!file_open(file, frpkg_path, file_flags)) {
    FREYA_LOG_ERROR("Failed to open frpkg file at '\%s\'", frpkg_path.c_str());
    return false;
  }

  group.frpkg_path = frpkg_path;

  // Read and check the version

  u8 version;
  file_read_bytes(file, &version, sizeof(version));

  if(version != FRPKG_VALID_VERSION) {
    FREYA_LOG_ERROR("Invalid frpkg version found \'%s\'. Expected \'%i\', but found \'%i\'", frpkg_path.c_str(), FRPKG_VALID_VERSION, version);
    return false;
  }

  // Read the sections count

  u8 sections_count;
  file_read_bytes(file, &sections_count, sizeof(sections_count));

  //
  // Read and add all of the assets in the package file
  //
  
  FREYA_LOG_DEBUG("Loading assets from \'%s\'...", frpkg_path.c_str());

  for(u8 i = 0; i < sections_count; i++) {
    // Read the asset type

    u8 asset_type;
    file_read_bytes(file, &asset_type, sizeof(asset_type));

    // Read the asset depending on its type

    switch(asset_type) {
      case ASSET_TYPE_TEXTURE:
        read_textures(file, group);
        break;
      case ASSET_TYPE_SHADER:
        // @TODO (Assets/shaders): Again, do we need this???
        break;
      case ASSET_TYPE_FONT:
        read_fonts(file, group);
        break;
      case ASSET_TYPE_AUDIO_BUFFER:
        read_audio_buffers(file, group);
        break;
      case ASSET_TYPE_UI_CONFIG:
        read_ui_config(file, group);
        break;
      case ASSET_TYPE_LUA:
        read_lua_state(file, group);
        break;
      default:
        break;
    }
  }

  // Send an event for the rest of the engine
  
  Event event = {
    .type     = EVENT_ASSET_GROUP_LOADED,
    .group_id = group_id, 
  };
  event_dispatch(event);

  // Done!
  
  file_close(file);
  return true;
}

const AssetID& asset_group_get_id(const AssetGroupID& group_id, const String& asset_name) {
  GROUP_CHECK(group_id);
  AssetGroup& group = s_manager.groups[group_id.get_id()];
 
  // The asset was not found
  
  if(group.named_ids.find(asset_name) == group.named_ids.end()) {
    FREYA_LOG_ERROR("Could not find asset \'%s\' in asset group \'%s\'", asset_name.c_str(), group.name.c_str());
    return group.named_ids["invalid"];
  }

  // Done!
  return group.named_ids[asset_name];
}

sg_buffer asset_group_get_buffer(const AssetID& id) {
  AssetGroup& group = s_manager.groups[id.get_group_id()];
  return get_asset(id, group.buffers, ASSET_TYPE_BUFFER);
}

Texture& asset_group_get_texture(const AssetID& id) {
  AssetGroup& group = s_manager.groups[id.get_group_id()];
  return get_asset(id, group.textures, ASSET_TYPE_TEXTURE);
}

sg_shader asset_group_get_shader(const AssetID& id) {
  AssetGroup& group = s_manager.groups[id.get_group_id()];
  return get_asset(id, group.shaders, ASSET_TYPE_SHADER);
}

Font* asset_group_get_font(const AssetID& id) {
  AssetGroup& group = s_manager.groups[id.get_group_id()];
  return get_asset(id, group.fonts, ASSET_TYPE_FONT);
}

const AudioBufferID& asset_group_get_audio_buffer(const AssetID& id) {
  AssetGroup& group = s_manager.groups[id.get_group_id()];
  return get_asset(id, group.audio_buffers, ASSET_TYPE_AUDIO_BUFFER);
}

UIConfig& asset_group_get_ui_config(const AssetID& id) {
  AssetGroup& group = s_manager.groups[id.get_group_id()];
  return get_asset(id, group.ui_configs, ASSET_TYPE_UI_CONFIG);
}

lua_State* asset_group_get_lua_state(const AssetID& id) {
  AssetGroup& group = s_manager.groups[id.get_group_id()];
  return get_asset(id, group.lua_states, ASSET_TYPE_LUA);
}

/// AssetGroupID functions
/// ----------------------------------------------------------------------

} // End of freya

//////////////////////////////////////////////////////////////////////////

#include "freya_assets.h"
#include "freya_event.h"
#include "freya_logger.h"
#include "freya_memory.h"

#include "frlist/frlist.h"
#include "loaders/asset_loaders.h"

//////////////////////////////////////////////////////////////////////////

namespace freya { // Start of freya

/// ----------------------------------------------------------------------
/// AssetGroup 
struct AssetGroup {
  String name; 
  AssetGroupID id;

  DynamicArray<GfxBuffer*> buffers;
  DynamicArray<GfxTexture*> textures;
  DynamicArray<GfxCubemap*> cubemaps;
  DynamicArray<GfxShader*> shaders;
  DynamicArray<AudioBufferID> audio_buffers;
  
  // DynamicArray<ShaderContext*> shader_contexts;
  // DynamicArray<Animation*> animations;
  // DynamicArray<Font*> fonts;

  HashMap<String, AssetID> named_ids;
};
/// AssetGroup 
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// AssetManager 
struct AssetManager {
  HashMap<i32, AssetGroup> groups;
};

static AssetManager s_manager;
/// AssetManager 
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Macros (Unfortunately)

#define DESTROY_CORE_ASSET_MAP(group, map, clear_func) { \
  for(auto& asset : group.map) {                         \
    clear_func(asset);                                   \
  }                                                      \
}

#define DESTROY_COMP_ASSET_MAP(group, map) { \
  for(auto& asset : group.map) {             \
    delete asset;                            \
  }                                          \
}

#define PUSH_ASSET(group, assets, asset, type, asset_id) {            \
  group.assets.push_back(asset);                                      \
  asset_id = AssetID(type, group.id, (i16)(group.assets.size() - 1)); \
}

#define GROUP_CHECK(group_id) FREYA_ASSERT(group_id != ASSET_GROUP_INVALID)

/// Macros (Unfortunately)
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Private functions 

static const char* buffer_type_str(const GfxBufferType type) {
  switch(type) {
    case GFX_BUFFER_VERTEX: 
      return "GFX_BUFFER_VERTEX";
    case GFX_BUFFER_INDEX: 
      return "GFX_BUFFER_INDEX";
    case GFX_BUFFER_UNIFORM: 
      return "GFX_BUFFER_UNIFORM";
    case GFX_BUFFER_SHADER_STORAGE: 
      return "GFX_BUFFER_SHADER_STORAGE";
    case GFX_BUFFER_DRAW_INDIRECT: 
      return "GFX_BUFFER_DRAW_INDIRECT";
    default:
      return "INVALID BUFFER TYPE";
  }
}

static const char* texture_type_str(const GfxTextureType type) {
  switch(type) {
    case GFX_TEXTURE_1D:
      return "GFX_TEXTURE_1D";
    case GFX_TEXTURE_2D:
      return "GFX_TEXTURE_2D";
    case GFX_TEXTURE_2D_PROXY:
      return "GFX_TEXTURE_2D_PROXY";
    case GFX_TEXTURE_3D:
      return "GFX_TEXTURE_3D";
    case GFX_TEXTURE_1D_ARRAY:
      return "GFX_TEXTURE_1D_ARRAY";
    case GFX_TEXTURE_1D_ARRAY_PROXY:
      return "GFX_TEXTURE_1D_ARRAY_PROXY";
    case GFX_TEXTURE_2D_ARRAY:
      return "GFX_TEXTURE_2D_ARRAY";
    case GFX_TEXTURE_IMAGE_1D:
      return "GFX_TEXTURE_IMAGE_1D";
    case GFX_TEXTURE_IMAGE_2D:
      return "GFX_TEXTURE_IMAGE_2D";
    case GFX_TEXTURE_IMAGE_3D:
      return "GFX_TEXTURE_IMAGE_3D";
    case GFX_TEXTURE_DEPTH_TARGET:
      return "GFX_TEXTURE_DEPTH_TARGET";
    case GFX_TEXTURE_STENCIL_TARGET:
      return "GFX_TEXTURE_STENCIL_TARGET";
    case GFX_TEXTURE_DEPTH_STENCIL_TARGET:
      return "GFX_TEXTURE_DEPTH_STENCIL_TARGET";
    default:
      return "INVALID TEXTURE TYPE";
  }
}

static const char* audio_format_str(const AudioBufferFormat format) {
  switch(format) {
    case AUDIO_BUFFER_FORMAT_U8:
      return "AUDIO_BUFFER_FORMAT_U8";
    case AUDIO_BUFFER_FORMAT_I16:
      return "AUDIO_BUFFER_FORMAT_I16";
    case AUDIO_BUFFER_FORMAT_F32:
      return "AUDIO_BUFFER_FORMAT_F32";
    default:
      return "INVALID AUDIO BUFFER FORMAT";
  }
}

template<typename T> 
static T get_asset(AssetID& id, DynamicArray<T>& asset, const AssetType type) {
  FREYA_DEBUG_ASSERT((id == type), "Invalid type when trying to retrieve a resource");
  FREYA_DEBUG_ASSERT((id >= 0 && id <= (i16)asset.size()), "Invalid ID when trying to retrieve a resource");

  return asset[id.get_id()];
}

static void build_textures(File& pkg_file, const ListSection& section) {
  FREYA_PROFILE_FUNCTION();

  for(const auto& path : section.assets) {
    // Write the name of the asset

    FilePath name = filepath_filename(path);
    file_write_bytes(pkg_file, name);

    // Load and save the asset

    GfxTextureDesc tex_desc{};

    texture_loader_load(path, &tex_desc);
    file_write_bytes(pkg_file, tex_desc);
    texture_loader_unload(tex_desc);
  }
}

static void build_shaders(File& pkg_file, const ListSection& section) {
  FREYA_PROFILE_FUNCTION();

  for(const auto& path : section.assets) {
    // Write the name of the asset

    FilePath name = filepath_filename(path);
    file_write_bytes(pkg_file, name);

    // Load and save the asset

    GfxShaderDesc shader_desc{};

    shader_loader_load(path, &shader_desc);
    file_write_bytes(pkg_file, shader_desc);
  }
}

static void build_audio_buffers(File& pkg_file, const ListSection& section) {
  FREYA_PROFILE_FUNCTION();

  for(const auto& path : section.assets) {
    // Write the name of the asset

    FilePath name = filepath_filename(path);
    file_write_bytes(pkg_file, name);

    // Load and save the asset

    GfxShaderDesc shader_desc{};

    shader_loader_load(path, &shader_desc);
    file_write_bytes(pkg_file, shader_desc);
  }
}

/// Private functions 
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Callbacks
/// @TODO (Assets/reloading)
/// Callbacks
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// AssetGroupID functions

AssetGroupID asset_group_create(const String& name) {
  AssetGroupID id               = AssetGroupID((i32)random_u32());
  s_manager.groups[id.get_id()] = AssetGroup {
    .name = name, 
    .id   = id,
  };

  FREYA_LOG_INFO("Successfully created an asset group \'%s\'", name.c_str());
  return id;
}

void asset_group_destroy(const AssetGroupID& group_id) {
  GROUP_CHECK(group_id);
  AssetGroup& group = s_manager.groups[group_id.get_id()];

  // Destroy compound assets
  // @TODO (Assets)
  
  // DESTROY_COMP_ASSET_MAP(group, shader_contexts);
  // DESTROY_COMP_ASSET_MAP(group, fonts);

  // Destroy core assets
  // @TODO (Assets)
  
  // DESTROY_CORE_ASSET_MAP(group, buffers, gfx_buffer_destroy);
  // DESTROY_CORE_ASSET_MAP(group, textures, gfx_texture_destroy);
  // DESTROY_CORE_ASSET_MAP(group, shaders, gfx_shader_destroy);
  // DESTROY_CORE_ASSET_MAP(group, audio_buffers, audio_buffer_destroy);

  // Done!

  FREYA_LOG_INFO("Asset group \'%s\' was successfully destroyed", group.name.c_str());
  s_manager.groups.erase(group_id.get_id());
}

bool asset_group_build(const AssetGroupID& group_id, const FilePath& list_path, const FilePath& output_path) {
  GROUP_CHECK(group_id);
  AssetGroup& group = s_manager.groups[group_id.get_id()];

  // Load the frlist file

  ListContext list_ctx;
  if(!frlist_load(list_ctx, list_path)) {
    FREYA_LOG_ERROR("Failed to read FRPKG file at \'%s\'", list_path.c_str());
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
  file_write_bytes(pkg_file, &FRPKG_VERSION, sizeof(FRPKG_VERSION));

  // Convert each asset in the list and write it to the newly-create frpkg file

  FREYA_LOG_TRACE("Converting assets from \'%s\' to \'%s\'", list_ctx.parent_dir.c_str(), output_path.c_str());

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
        build_shaders(pkg_file, section);
        break;
      case ASSET_TYPE_FONT:
        // @TODO (Assets)
        break;
      case ASSET_TYPE_AUDIO_BUFFER:
        build_audio_buffers(pkg_file, section);
        break;
      default:
        break;
    }
  }

  // Done!
 
  file_close(pkg_file);
  FREYA_LOG_DEBUG("Successfully built frpkg at \'%s\'!", output_path.c_str());
  
  return true;
}

bool asset_group_load_package(const AssetGroupID& group_id, const FilePath& frpkg_path) {
  GROUP_CHECK(group_id);
  AssetGroup& group = s_manager.groups[group_id.get_id()];

  // @TODO (Assets)
  
  // Done!
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

/// AssetGroupID functions
/// ----------------------------------------------------------------------

} // End of freya

//////////////////////////////////////////////////////////////////////////

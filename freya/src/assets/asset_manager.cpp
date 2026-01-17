#include "freya_assets.h"
#include "freya_event.h"
#include "freya_logger.h"
#include "freya_memory.h"
#include "freya_render.h"

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
  
  DynamicArray<ShaderContext*> shader_contexts;
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
static T get_asset(const AssetID& id, DynamicArray<T>& asset, const AssetType type) {
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

    // Load and save the asset

    GfxTextureDesc tex_desc{};

    texture_loader_load(path, &tex_desc);
    file_write_bytes(pkg_file, tex_desc);
    texture_loader_unload(tex_desc);
  }
}

static void build_shaders(File& pkg_file, const ListSection& section) {
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

    GfxShaderDesc shader_desc{};

    shader_loader_load(path, &shader_desc);
    file_write_bytes(pkg_file, shader_desc);
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

    // Load and save the asset

    AudioBufferDesc audio_desc{};

    audio_loader_load(path, &audio_desc);
    file_write_bytes(pkg_file, audio_desc);
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

    // Read the texture desc

    GfxTextureDesc desc;
    desc.type = GFX_TEXTURE_2D;

    file_read_bytes(file, &desc);

    // Add the texture to the group
    group.named_ids[name] = asset_group_push_texture(group.id, desc); 

    // Get rid of the texture data on the CPU-side
    memory_free(desc.data); 

    FREYA_LOG_DEBUG("Loaded texture \'%s\' from frpkg ", name.c_str());
  }
}

static void read_shaders(File& file, AssetGroup& group) {
  FREYA_PROFILE_FUNCTION();
  
  // Read the count

  u16 count;
  file_read_bytes(file, &count, sizeof(count));

  // Read the asset

  for(u16 i = 0; i < count; i++) {
    // Read the name

    String name;
    file_read_bytes(file, &name);

    // Read the shader desc

    GfxShaderDesc desc;
    file_read_bytes(file, &desc);

    // Add the shader to the group
    group.named_ids[name] = asset_group_push_shader(group.id, desc); 

    FREYA_LOG_DEBUG("Loaded shader \'%s\' from frpkg ", name.c_str());
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

    // Read the audio desc

    AudioBufferDesc desc;
    file_read_bytes(file, &desc);

    // Add the audio buffer to the group
    group.named_ids[name] = asset_group_push_audio_buffer(group.id, desc); 

    // Get rid of the audio data on the CPU-side
    memory_free(desc.data); 

    FREYA_LOG_DEBUG("Loaded audio buffer \'%s\' from frpkg ", name.c_str());
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
  
  DESTROY_COMP_ASSET_MAP(group, shader_contexts);

  // Destroy core assets
  
  DESTROY_CORE_ASSET_MAP(group, buffers, gfx_buffer_destroy);
  DESTROY_CORE_ASSET_MAP(group, textures, gfx_texture_destroy);
  DESTROY_CORE_ASSET_MAP(group, shaders, gfx_shader_destroy);
  DESTROY_CORE_ASSET_MAP(group, audio_buffers, audio_buffer_destroy);

  // Done!

  FREYA_LOG_INFO("Asset group \'%s\' was successfully destroyed", group.name.c_str());
  s_manager.groups.erase(group_id.get_id());
}

bool asset_group_build(const AssetGroupID& group_id, const FilePath& list_path, const FilePath& output_path) {
  GROUP_CHECK(group_id);
  AssetGroup& group = s_manager.groups[group_id.get_id()];

  // We need to check if it's even needed to build the package. 
  // For example, it might already be up-to-date.

  if(!can_build_frpkg(filepath_parent_path(list_path), output_path)) {
    FREYA_LOG_TRACE("Frpkg at \'%s\' is up-to-date", output_path.c_str());
    return true; // No need to build the package... 
  }

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
  file_write_bytes(pkg_file, &FRPKG_VALID_VERSION, sizeof(FRPKG_VALID_VERSION));

  // Write the number of sections
  
  u8 sections_count = (u8)list_ctx.sections.size();
  file_write_bytes(pkg_file, &sections_count, sizeof(sections_count));

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

AssetID asset_group_push_buffer(const AssetGroupID& group_id, const GfxBufferDesc& buff_desc) {
  GROUP_CHECK(group_id);
  AssetGroup& group = s_manager.groups[group_id.get_id()];

  // Create and push the buffer

  GfxBuffer* buffer = gfx_buffer_create(renderer_get_context()); 

  AssetID id;
  PUSH_ASSET(group, buffers, buffer, ASSET_TYPE_BUFFER, id);

  // Load the buffer's data
  gfx_buffer_load(buffer, buff_desc);

  // Some useful debug info
  
  FREYA_LOG_DEBUG("Group \'%s\' pushed buffer:", group.name.c_str());
  FREYA_LOG_DEBUG("     Size = %zu", buff_desc.size);
  FREYA_LOG_DEBUG("     Type = %s", buffer_type_str(buff_desc.type));

  // Done!
  return id;
}

AssetID asset_group_push_texture(const AssetGroupID& group_id, const GfxTextureDesc& tex_desc) {
  GROUP_CHECK(group_id);
  AssetGroup& group = s_manager.groups[group_id.get_id()];
  
  // Create and push the texture
  
  GfxTexture* texture = gfx_texture_create(renderer_get_context(), tex_desc.type);

  AssetID id; 
  PUSH_ASSET(group, textures, texture, ASSET_TYPE_TEXTURE, id);
 
  // Load the texture's data
  gfx_texture_load(texture, tex_desc);
 
  // Some useful debug info
  
  FREYA_LOG_DEBUG("Group \'%s\' pushed texture:", group.name.c_str());
  FREYA_LOG_DEBUG("     Size = %i X %i", tex_desc.width, tex_desc.height);
  FREYA_LOG_DEBUG("     Type = %s", texture_type_str(tex_desc.type));
 
  // Done!
  return id;
}

AssetID asset_group_push_shader(const AssetGroupID& group_id, const GfxShaderDesc& shader_desc) {
  GROUP_CHECK(group_id);
  AssetGroup& group = s_manager.groups[group_id.get_id()];
  
  // Create and load the shader
  
  GfxShader* shader = gfx_shader_create(renderer_get_context());
  gfx_shader_load(shader, shader_desc); 

  // New shader ID

  AssetID id; 
  PUSH_ASSET(group, shaders, shader, ASSET_TYPE_SHADER, id);
  
  // Some useful debug info
  
  FREYA_LOG_DEBUG("Group \'%s\' pushed shader:", group.name.c_str());
  if(!shader_desc.compute_source.empty()) {
    FREYA_LOG_DEBUG("     Compute source length = %zu", shader_desc.compute_source.size());
  }
  else {
    FREYA_LOG_DEBUG("     Vertex source length = %zu", shader_desc.vertex_source.size());
    FREYA_LOG_DEBUG("     Pixel source length  = %zu", shader_desc.pixel_source.size());
  }

  // Done!
  return id;
}

AssetID asset_group_push_shader_context(const AssetGroupID& group_id, const AssetID& shader_id) {
  GROUP_CHECK(group_id);
  AssetGroup& group = s_manager.groups[group_id.get_id()];
  
  // Allocate the context
  
  ShaderContext* ctx = new ShaderContext{};
  ctx->shader        = asset_group_get_shader(shader_id);

  // Create the context
  
  AssetID id; 
  PUSH_ASSET(group, shader_contexts, ctx, ASSET_TYPE_SHADER_CONTEXT, id);

  // Query the shader for uniform information

  GfxShaderQueryDesc query_desc = {};
  gfx_shader_query(ctx->shader, &query_desc);

  for(sizei i = 0; i < query_desc.uniforms_count; i++) {
    GfxUniformDesc* uniform = &query_desc.active_uniforms[i];
    if(uniform->location == -1) { // Invalid uniform. Why??
      continue;
    }
    
    ctx->uniforms_cache[uniform->name] = uniform->location;
  }

  // Some useful debug info
  
  FREYA_LOG_DEBUG("Group \'%s\' pushed shader context:", group.name.c_str());
  FREYA_LOG_DEBUG("     Attributes count      = %i", query_desc.attributes_count);
  FREYA_LOG_DEBUG("     Uniforms count        = %i", query_desc.uniforms_count);
  FREYA_LOG_DEBUG("     Uniform buffers count = %i", query_desc.uniform_blocks_count);
  
  // Done!
  return id;
}

AssetID asset_group_push_shader_context(const AssetGroupID& group_id, const GfxShaderDesc& shader_desc) {
  // Create a new shader
  AssetID shader_id = asset_group_push_shader(group_id, shader_desc);

  // Done!
  return asset_group_push_shader_context(group_id, shader_id);
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
  
  FREYA_LOG_TRACE("Loading assets from \'%s\'...", frpkg_path.c_str());

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
        read_shaders(file, group);
        break;
      case ASSET_TYPE_AUDIO_BUFFER:
        read_audio_buffers(file, group);
        break;
      default:
        break;
    }
  }
  
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

GfxBuffer* asset_group_get_buffer(const AssetID& id) {
  AssetGroup& group = s_manager.groups[id.get_group_id()];
  return get_asset(id, group.buffers, ASSET_TYPE_BUFFER);
}

GfxTexture* asset_group_get_texture(const AssetID& id) {
  AssetGroup& group = s_manager.groups[id.get_group_id()];
  return get_asset(id, group.textures, ASSET_TYPE_TEXTURE);
}

GfxShader* asset_group_get_shader(const AssetID& id) {
  AssetGroup& group = s_manager.groups[id.get_group_id()];
  return get_asset(id, group.shaders, ASSET_TYPE_SHADER);
}

ShaderContext* asset_group_get_shader_context(const AssetID& id) {
  AssetGroup& group = s_manager.groups[id.get_group_id()];
  return get_asset(id, group.shader_contexts, ASSET_TYPE_SHADER_CONTEXT);
}

const AudioBufferID& asset_group_get_audio_buffer(const AssetID& id) {
  AssetGroup& group = s_manager.groups[id.get_group_id()];
  return get_asset(id, group.audio_buffers, ASSET_TYPE_AUDIO_BUFFER);
}

/// AssetGroupID functions
/// ----------------------------------------------------------------------

} // End of freya

//////////////////////////////////////////////////////////////////////////

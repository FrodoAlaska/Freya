#include "asset_loaders.h"

#include "freya_assets.h"
#include "freya_logger.h"
#include "freya_memory.h"

/// ----------------------------------------------------------------------
/// Private functions

static const freya::sizei compile_vertex_shader(const freya::String& src, freya::GfxShaderDesc* shader) {
  // Getting the vertex source until the next identifier

  freya::sizei begin = src.find_first_of('#'); 
  for(freya::sizei i = begin; i < src.size(); i++) { // @NOTE: Apparently the combination of `substr` and `find_first_of` does not work here
    if(src[i] == '@') {
      break;
    }

    shader->vertex_source += src[i];
  }
 
  // Done!
  return shader->vertex_source.size();
}

/// Private functions
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Shader loader functions

bool shader_loader_load(const freya::FilePath& path, freya::GfxShaderDesc* shader_desc) {
  // Open the shader file

  freya::File file;
  if(!freya::file_open(file, path, (freya::i32)freya::FILE_OPEN_READ)) {
    FREYA_LOG_ERROR("Failed to open shader file at \'%s\'", path.c_str());
    return false;
  }
  
  // Read the string from the file
  
  freya::String shader_src; 
  freya::file_read_string(file, &shader_src);
  freya::file_close(file);

  // Trying to seperate the shader into two
  freya::sizei iden_pos = shader_src.find_first_of('@');

  // Depedning on the shader type, we compile the shader differently

  switch(shader_src[iden_pos + 1]) {
    case 'c': // Compute
      shader_desc->compute_source = shader_src.substr(shader_src.find_first_of('#'));
      break;
    case 'v': { // Vertex and pixel shaders
      freya::sizei current_pos  = compile_vertex_shader(shader_src, shader_desc);
      shader_desc->pixel_source = shader_src.substr(shader_src.find_first_of('#', current_pos)); 
    } break;
    default: 
      FREYA_LOG_ERROR("Could not find shader identifiers in shader at \'%s\'", path.c_str());
      break;
  }

  // Done!
 
  FREYA_LOG_TRACE("Successfully loaded shader at '\%s\'", path.c_str());
  return true;
}

/// Shader loader functions
/// ----------------------------------------------------------------------

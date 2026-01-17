#include "asset_loaders.h"

#include "freya_assets.h"
#include "freya_logger.h"

#include <stb/stb_image.h>

/// ----------------------------------------------------------------------
/// Private functions

static bool check_valid_extension(const freya::FilePath& ext) {
  return ext == ".png"  ||  
         ext == ".jpg"  ||  
         ext == ".jpeg" ||  
         ext == ".bmp"  ||  
         ext == ".psd"  ||  
         ext == ".tga"  ||  
         ext == ".gif"  ||  
         ext == ".hdr"  ||  
         ext == ".pic"  ||  
         ext == ".ppm"  ||  
         ext == ".pgm";
}

/// Private functions
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Texture loader functions

bool texture_loader_load(const freya::FilePath& path, freya::GfxTextureDesc* tex_desc) {
  // Sanity check

  if(!check_valid_extension(freya::filepath_extension(path))) {
    FREYA_LOG_WARN("Invalid texture file found at \'%s\'!", path.c_str());
    return false;
  }

  // Loading the texture 

  freya::i32 width, height; 
  if(stbi_is_hdr(path.c_str())) {
    tex_desc->format = freya::GFX_TEXTURE_FORMAT_RGBA16F;
    tex_desc->data   = stbi_loadf(path.c_str(), &width, &height, NULL, 4);
  }
  else {
    tex_desc->format = freya::GFX_TEXTURE_FORMAT_RGBA8;
    tex_desc->data   = stbi_load(path.c_str(), &width, &height, NULL, 4);
  }

  // Fuck!

  if(!tex_desc->data) {
    FREYA_LOG_ERROR("Could not load texture at \'%s'\, %s", path.c_str(), stbi_failure_reason());
    return false;
  }

  // Filling the rest of the desc

  tex_desc->width  = (freya::u32)width;
  tex_desc->height = (freya::u32)height;

  // Done!
 
  FREYA_LOG_TRACE("Successfully loaded texture at '\%s\'", path.c_str());
  return true;
}

void texture_loader_unload(freya::GfxTextureDesc& tex_desc) {
  if(!tex_desc.data) {
    return;
  }
  
  stbi_image_free(tex_desc.data);
}

/// Texture loader functions
/// ----------------------------------------------------------------------

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

bool texture_loader_load(const freya::FilePath& path, sg_image_desc& out_img) {
  // Sanity check

  if(!check_valid_extension(freya::filepath_extension(path))) {
    FREYA_LOG_WARN("Invalid texture file found at \'%s\'!", path.c_str());
    return false;
  }

  // Loading the texture 

  freya::i32 width, height; 
  if(stbi_is_hdr(path.c_str())) {
    out_img.pixel_format           = SG_PIXELFORMAT_RGBA32F;
    out_img.data.mip_levels[0].ptr = stbi_loadf(path.c_str(), &width, &height, NULL, 4);
  }
  else {
    out_img.pixel_format           = SG_PIXELFORMAT_RGBA8;
    out_img.data.mip_levels[0].ptr = stbi_load(path.c_str(), &width, &height, NULL, 4);
  }

  // Fuck!

  if(!out_img.data.mip_levels[0].ptr) {
    FREYA_LOG_ERROR("Could not load texture at \'%s'\, %s", path.c_str(), stbi_failure_reason());
    return false;
  }

  // Filling the rest of the desc

  out_img.width  = width;
  out_img.height = height;

  // Done!
 
  FREYA_LOG_TRACE("Successfully loaded texture at '\%s\'", path.c_str());
  return true;
}

/// Texture loader functions
/// ----------------------------------------------------------------------

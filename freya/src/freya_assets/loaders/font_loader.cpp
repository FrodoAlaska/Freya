#include "asset_loaders.h"

#include "freya_assets.h"
#include "freya_logger.h"

/// ----------------------------------------------------------------------
/// Font loader functions

bool font_loader_load(const freya::FilePath& path, freya::DynamicArray<freya::u8>& font_data) {
  // Open the file

  freya::File file;
  freya::i32 file_flags = (freya::i32)(freya::FILE_OPEN_READ | freya::FILE_OPEN_BINARY);

  if(!freya::file_open(file, path, file_flags)) {
    FREYA_LOG_ERROR("Could not load font at \'%s\'", path.c_str());
    return false;
  } 

  // Load the actual data from the file

  freya::sizei data_size = freya::filesystem_get_size(path);
  font_data.resize(data_size);

  freya::file_read_bytes(file, font_data.data(), data_size); 

  // Done!
   
  FREYA_LOG_TRACE("Successfully loaded font at '\%s\'", path.c_str());
  return true;
}

/// Font loader functions
/// ----------------------------------------------------------------------

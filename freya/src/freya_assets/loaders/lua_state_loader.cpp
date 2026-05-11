#include "asset_loaders.h"

#include "freya_logger.h"

/// ----------------------------------------------------------------------
/// LUA state loader functions

bool lua_state_loader_load(const freya::FilePath& path, freya::String* out_src) {
  // Open the LUA file

  freya::File file;
  if(!freya::file_open(file, path, (freya::i32)freya::FILE_OPEN_READ)) {
    FREYA_LOG_ERROR("Failed to open LUA file at \'%s\'", path.c_str());
    return false;
  }
  
  // Read the string from the file
  
  freya::file_read_string(file, out_src);
  freya::file_close(file);

  // Done!
  
  FREYA_LOG_TRACE("Successfully loaded LUA file at '\%s\'", path.c_str());
  return true;
}

/// LUA state loader functions
/// ----------------------------------------------------------------------

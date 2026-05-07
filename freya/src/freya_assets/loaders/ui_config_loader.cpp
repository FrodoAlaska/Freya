#include "asset_loaders.h"

#include "freya_ui.h"
#include "freya_logger.h"

/// ----------------------------------------------------------------------
/// UIConfig loader functions

bool ui_config_loader_load(const freya::FilePath& path, freya::UIConfig* ui_cfg) {
  // Open the UI config file

  freya::File file;
  if(!freya::file_open(file, path, (freya::i32)freya::FILE_OPEN_READ)) {
    FREYA_LOG_ERROR("Failed to open UI config file at \'%s\'", path.c_str());
    return false;
  }
  
  // Read the string from the file
  
  freya::file_read_string(file, &ui_cfg->html_source);
  freya::file_close(file);

  // Done!
  
  FREYA_LOG_TRACE("Successfully loaded UI config at '\%s\'", path.c_str());
  return true;
}

/// UIConfig loader functions
/// ----------------------------------------------------------------------

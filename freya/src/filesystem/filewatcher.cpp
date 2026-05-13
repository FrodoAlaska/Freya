#include "freya_file.h"
#include "freya_logger.h"

//////////////////////////////////////////////////////////////////////////

namespace freya { // Start of freya

///---------------------------------------------------------------------------------------------------------------------
/// FileWatcher functions

FileWatcher* filewatcher_create(const FilePath& path, FileWatchFunc watch_func) {
#if FREYA_PLATFORM_WEB != 1
  FileWatcher* watcher = new FileWatcher(path, [=](const FilePath& path, const filewatch::Event event){
    watch_func(path, (FileStatus)event); 
  });
  return watcher;
#else 
  FREYA_ASSERT_LOG(false, "The file watcher is not available for web builds... sorry");
  return nullptr;
#endif
}

void filewatcher_destroy(FileWatcher* watcher) {
#if FREYA_PLATFORM_WEB != 1
  delete watcher;
#endif
}

/// FileWatcher functions
///---------------------------------------------------------------------------------------------------------------------

} // End of freya

//////////////////////////////////////////////////////////////////////////

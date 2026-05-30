#pragma once

#include "freya_math.h"

//////////////////////////////////////////////////////////////////////////

namespace freya { // Start of freya

///---------------------------------------------------------------------------------------------------------------------
/// FileOpenMode
enum FileOpenMode {
  /// Open a file in read-only mode.
  FILE_OPEN_READ       = 6 << 0,
 
  /// Open a file in write-only mode.
  FILE_OPEN_WRITE      = 6 << 1,
 
  /// Open a binary file.
  FILE_OPEN_BINARY     = 6 << 2,
 
  /// Open a file and append any extra data at the end of the file.
  FILE_OPEN_APPEND     = 6 << 3,
 
  /// Open a file and remove any existing data within.
  FILE_OPEN_TRUNCATE   = 6 << 4,
 
  /// Open a file and start at the end.
  FILE_OPEN_AT_END     = 6 << 5,

  /// Open a file in read and write mode.
  FILE_OPEN_READ_WRITE = 6 << 6
};
/// FileOpenMode
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// FileStatus
enum FileStatus {
  /// File status to indicate a new file. 
  FILE_STATUS_ADDED, 
  
  /// File status to indicate a removed file.
  FILE_STATUS_REMOVED,
  
  /// File status to indicate a modified file.
  FILE_STATUS_MODIFIED,
  
  /// File status to indicate a file that was renamed, 
  /// returning the old name.
  FILE_STATUS_RENAMED_OLD,
  
  /// File status to indicate a file that was renamed, 
  /// returning the new name.
  FILE_STATUS_RENAMED_NEW,
};
/// FileStatus
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// FilePath
using FilePath = String;
/// FilePath
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// File
using File = std::fstream;
/// File
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// FileTimePoint
using FileTimePoint = std::filesystem::file_time_type;
/// FileTimePoint
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// FileWatcher
#if FREYA_PLATFORM_WEB != 1
  using FileWatcher = filewatch::FileWatch<FilePath>;
#else
  using FileWatcher = i32;
#endif
/// FileWatcher
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// FileIterateFunc
using FileIterateFunc = std::function<bool(const FilePath& base_dir, const FilePath& current_path, void* user_data)>;
/// FileIterateFunc
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// FileWatchFunc
using FileWatchFunc = std::function<void(const FilePath& path, const FileStatus status)>;
/// FileWatchFunc
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Filesystem functions

/// Iterate through the given `dir`, calling `iter_func` for each entry and passing `user_data`. 
FREYA_API void filesystem_directory_iterate(const FilePath& dir, const FileIterateFunc& iter_func, const void* user_data = nullptr);

/// Recursively iterate through the given `dir`, calling `iter_func` for each entry and passing `user_data`.
FREYA_API void filesystem_directory_recurse_iterate(const FilePath& dir, const FileIterateFunc& iter_func, const void* user_data = nullptr);

/// Get the current full path of the running process.
FREYA_API FilePath filesystem_current_path();

/// Check if the file at `path` exists.
FREYA_API bool filesystem_exists(const FilePath& path);

/// Return the size in bytes of the file found at `path`. 
FREYA_API sizei filesystem_get_size(const FilePath& path);

/// Returns `true` if the file at `path` is completely empty. Otherwise, the functions returns `false`.
FREYA_API bool filesystem_is_empty(const FilePath& path);

/// Create a directory with the name `dir_name`. 
/// Returns `true` if the directory was created. Otherwise, the function returns `false`.
FREYA_API bool filesystem_create_directory(const FilePath& dir_name);

/// Create all of the directories (if they don't exist) in the given `dir_path`. 
/// Returns `true` if the directories were created. Otherwise, the function returns `false`.
FREYA_API bool filesystem_create_directories(const FilePath& dir_path);

/// Get the last write time of the given `path`
FREYA_API FileTimePoint filesystem_get_last_write_time(const FilePath& path);

/// Filesystem functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// FilePath functions

/// Append the given `other` to `base` and return the result.
///
/// @NOTE: This function will adhere to the specific operating systems's seperator.
FREYA_API FilePath filepath_append(const FilePath& base, const FilePath& other);

/// Retrieve the root name (for example, `C:` on Windows) of `path`.
/// 
/// @EXAMPLE: The root namt would be `C:` on Windows.
FREYA_API FilePath filepath_root_name(const FilePath& path);

/// Retrieve the root directory of `path`.
/// 
/// @EXAMPLE: The root directory would be `\` on Windows.
FREYA_API FilePath filepath_root_dir(const FilePath& path);

/// Retrieve the full root path of `path`.
/// 
/// @EXAMPLE: The root path would be `C:\` on Windows.
FREYA_API FilePath filepath_root_path(const FilePath& path);

/// Retrieve the full relative path of `path`. 
///
/// @EXAMPLE: The relative path of `C:\res\shaders\shader.glsl` would be `res\shaders\shader.glsl`.
FREYA_API FilePath filepath_relative_path(const FilePath& path);

/// Retrieve the full parent path of `path`. 
///
/// @EXAMPLE: The parent path of `res/shaders/shader.glsl` would be `res/shaders/`.
FREYA_API FilePath filepath_parent_path(const FilePath& path);

/// Retrieve the filename of `path`.
///
/// @EXAMPLE: The filename of `res/shaders/shader.glsl` would be `shader.glsl`.
FREYA_API FilePath filepath_filename(const FilePath& path);

/// Retrieve the stem of `path`.
///
/// @EXAMPLE: The stem of `res/shaders/shader.glsl` would be `shader`.
FREYA_API FilePath filepath_stem(const FilePath& path);

/// Retrieve the extension of `path`.
///
/// @EXAMPLE: The extension of `res/shaders/shader.glsl` would be `.glsl`.
FREYA_API FilePath filepath_extension(const FilePath& path);

/// Set the filename of `path` to the given `name`.
FREYA_API void filepath_set_filename(FilePath& path, const FilePath& name);

/// Set the extension of `path` to the given `ext`.
FREYA_API void filepath_set_extension(FilePath& path, const FilePath& ext);

/// Remove the filename of `path`. 
///
/// @EXAMPLE: The path `res/shaders/shader.glsl` will become `res/shaders/`
FREYA_API void filepath_remove_filename(FilePath& path);

/// Check if `path` has a root name.
FREYA_API bool filepath_has_root_name(const FilePath& path);

/// Check if `path` has a root directory.
FREYA_API bool filepath_has_root_dir(const FilePath& path);

/// Check if `path` has a root path.
FREYA_API bool filepath_has_root_path(const FilePath& path);

/// Check if `path` has a ralative path.
FREYA_API bool filepath_has_relative_path(const FilePath& path);

/// Check if `path` has a parent path.
FREYA_API bool filepath_has_parent_path(const FilePath& path);

/// Check if `path` has a filename.
FREYA_API bool filepath_has_filename(const FilePath& path);

/// Check if `path` has a stem.
FREYA_API bool filepath_has_stem(const FilePath& path);

/// Check if `path` has an extension.
FREYA_API bool filepath_has_extension(const FilePath& path);

/// Check if `path` is an empty string.
FREYA_API bool filepath_is_empty(const FilePath& path);

/// Check if `path` is a relative path.
FREYA_API bool filepath_is_relative(const FilePath& path);

/// Check if `path` is an absolute path.
FREYA_API bool filepath_is_absolute(const FilePath& path);

/// Check if `path` is a directory.
FREYA_API bool filepath_is_dir(const FilePath& path);

/// FilePath functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// File functions

/// Open `file` with OR'd flags `mode` at C-string `path`, and return `true` if 
/// the operation was successfull and `false` otherwise.
///
/// `mode`:
///   - `FILE_OPEN_READ`       = Open `file` in read-only mode.
///   - `FILE_OPEN_WRITE`      = Open `file` in write-only mode.
///   - `FILE_OPEN_BINARY`     = Open `file` in binary mode.
///   - `FILE_OPEN_APPEND`     = Open `file` and append any new data at the end.
///   - `FILE_OPEN_TRUNCATE`   = Open `file` and delete any existing data.
///   - `FILE_OPEN_AT_END`     = Open `file` and start at the very end.
///   - `FILE_OPEN_READ_WRITE` = Open `file` in read and write mode.
FREYA_API bool file_open(File& file, const char* path, const i32 mode);

/// Open `file` with OR'd flags `mode` at `FilePath` `path`, and return `true` if 
/// the operation was successfull and `false` otherwise.
///
/// `mode`:
///   - `FILE_OPEN_READ`       = Open `file` in read-only mode.
///   - `FILE_OPEN_WRITE`      = Open `file` in write-only mode.
///   - `FILE_OPEN_BINARY`     = Open `file` in binary mode.
///   - `FILE_OPEN_APPEND`     = Open `file` and append any new data at the end.
///   - `FILE_OPEN_TRUNCATE`   = Open `file` and delete any existing data.
///   - `FILE_OPEN_AT_END`     = Open `file` and start at the very end.
///   - `FILE_OPEN_READ_WRITE` = Open `file` in read and write mode.
FREYA_API bool file_open(File& file, const FilePath& path, const i32 mode);

/// Close `file` if it is open.
FREYA_API void file_close(File& file);

/// Return `ture` if `file` is currently open and `false` otherwise.
FREYA_API bool file_is_open(File& file);

/// Seek the write pointer in `file` to `pos`.
FREYA_API void file_seek_write(File& file, const sizei pos);

/// Seek the read pointer in `file` to `pos`.
FREYA_API void file_seek_read(File& file, const sizei pos);

/// Return the current position of the write pointer in `file`.
FREYA_API const sizei file_tell_write(File& file);

/// Return the current position of the read pointer in `file`.
FREYA_API const sizei file_tell_read(File& file);

/// Write `buff` with `size` in bytes into `file` 
/// and return the amount of bytes written.
///
/// @NOTE: This function will raise an error if `file` is not opened.
FREYA_API const sizei file_write_bytes(File& file, const void* buff, const sizei buff_size);

/// Write the given `str` into `file` as a stream of bytes.
///
/// @NOTE: This function will raise an error if `file` is not opened.
FREYA_API void file_write_bytes(File& file, const String& str);

/// Write the given `string` into `file` as a string representation.
///
/// @NOTE: This function will raise an error if `file` is not opened.
FREYA_API void file_write_string(File& file, const String& string);

/// Read data into `out_buff` with `size` in bytes from `file` 
/// and return the amount of bytes read.
///
/// @NOTE: This function will raise an error if `file` is not opened.
FREYA_API const sizei file_read_bytes(File& file, void* out_buff, const sizei size);

/// Return a `String` from `file` and it into `str`.
///
/// @NOTE: This function will raise an error if `file` is not opened.
FREYA_API void file_read_bytes(File& file, String* str);

/// Read the WHOLE `file` as a `String` and save it into `str`.
///
/// @NOTE: This function will raise an error if `file` is not opened.
FREYA_API void file_read_string(File& file, String* str);

/// File functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// FileWatcher functions

/// Create a new `FileWatcher`, marking the `path` to watch, and calling the given `watch_func` 
/// when any interesting events occur at `path`.
FREYA_API FileWatcher* filewatcher_create(const FilePath& path, FileWatchFunc watch_func);

/// Destroy/de-allocate the given `watcher` object.
FREYA_API void filewatcher_destroy(FileWatcher* watcher);

/// FileWatcher functions
///---------------------------------------------------------------------------------------------------------------------

} // End of freya

//////////////////////////////////////////////////////////////////////////

#include "freya_file.h"
#include "freya_logger.h"

#include <sstream>

//////////////////////////////////////////////////////////////////////////

namespace freya { // Start of freya

///---------------------------------------------------------------------------------------------------------------------
/// Private functions

static std::ios::openmode get_mode(const i32 mode) {
  std::ios::openmode cpp_mode = (std::ios::openmode)0;

  if(IS_BIT_SET(mode, FILE_OPEN_READ)) {
    cpp_mode = (std::ios::openmode)(cpp_mode | std::ios::in);
  }
  
  if(IS_BIT_SET(mode, FILE_OPEN_WRITE)) {
    cpp_mode = (std::ios::openmode)(cpp_mode | std::ios::out);
  }
  
  if(IS_BIT_SET(mode, FILE_OPEN_BINARY)) {
    cpp_mode = (std::ios::openmode)(cpp_mode | std::ios::binary);
  }
  
  if(IS_BIT_SET(mode, FILE_OPEN_APPEND)) {
    cpp_mode = (std::ios::openmode)(cpp_mode | std::ios::app);
  }
  
  if(IS_BIT_SET(mode, FILE_OPEN_TRUNCATE)) {
    cpp_mode = (std::ios::openmode)(cpp_mode | std::ios::trunc);
  }
  
  if(IS_BIT_SET(mode, FILE_OPEN_AT_END)) {
    cpp_mode = (std::ios::openmode)(cpp_mode | std::ios::ate);
  }
  
  if(IS_BIT_SET(mode, FILE_OPEN_READ_WRITE)) {
    cpp_mode = (std::ios::openmode)(cpp_mode | (std::ios::in | std::ios::out));
  }

  return cpp_mode;
}

/// Private functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// File functions

bool file_open(File& file, const char* path, const i32 mode) {
  FREYA_DEBUG_ASSERT(file, "Cannot open an invalid File handle");

  file.open(path, get_mode(mode));
  return file.is_open();
}

bool file_open(File& file, const FilePath& path, const i32 mode) {
  FREYA_DEBUG_ASSERT(file, "Cannot open an invalid File handle");

  file.open(path, get_mode(mode));
  return file.is_open();
}

void file_close(File& file) {
  file.close();
}

bool file_is_open(File& file) {
  return file.is_open();
}

void file_seek_write(File& file, const sizei pos) {
  FREYA_DEBUG_ASSERT(file.is_open(), "Cannot perform an operation on an unopened file");
  
  file.seekp(pos);
}

void file_seek_read(File& file, const sizei pos) {
  FREYA_DEBUG_ASSERT(file.is_open(), "Cannot perform an operation on an unopened file");
  
  file.seekg(pos);
}

const sizei file_tell_write(File& file) {
  FREYA_DEBUG_ASSERT(file.is_open(), "Cannot perform an operation on an unopened file");
  
  return file.tellp();
}

const sizei file_tell_read(File& file) {
  FREYA_DEBUG_ASSERT(file.is_open(), "Cannot perform an operation on an unopened file");
  
  return file.tellg();
}

const sizei file_write_bytes(File& file, const void* buff, const sizei buff_size) {
  FREYA_DEBUG_ASSERT(file.is_open(), "Cannot perform an operation on an unopened file");
  
  file.write((char*)buff, buff_size);
  return buff_size;
}

void file_write_bytes(File& file, const String& str) {
  FREYA_DEBUG_ASSERT(file.is_open(), "Cannot perform an operation on an unopened file");

  // Write the size of the string

  u32 str_len = (u32)str.size();
  file_write_bytes(file, &str_len, sizeof(str_len));

  // Write the string itself (only if it's valid)
 
  if(str_len > 0) {
    file_write_bytes(file, str.data(), str_len);
  }
}

void file_write_string(File& file, const String& string) {
  FREYA_DEBUG_ASSERT(file.is_open(), "Cannot perform an operation on an unopened file");

  file << string;
}

const sizei file_read_bytes(File& file, void* out_buff, const sizei size) {
  FREYA_DEBUG_ASSERT(file.is_open(), "Cannot perform an operation on an unopened file");
  
  file.read((char*)out_buff, size);
  return size;
}

void file_read_bytes(File& file, String* str) {
  FREYA_DEBUG_ASSERT(file.is_open(), "Cannot perform an operation on an unopened file");

  // Read the length

  u32 str_len;
  file_read_bytes(file, &str_len, sizeof(str_len));
  
  // Read the string (only if it's valid)

  if(str_len > 0) {
    str->resize(str_len);
    file_read_bytes(file, str->data(), str_len);
  }
}

void file_read_string(File& file, String* str) {
  FREYA_DEBUG_ASSERT(file.is_open(), "Cannot perform an operation on an unopened file");

  std::stringstream ss;
  ss << file.rdbuf();

  *str = ss.str();
}

/// File functions
///---------------------------------------------------------------------------------------------------------------------

} // End of freya

//////////////////////////////////////////////////////////////////////////

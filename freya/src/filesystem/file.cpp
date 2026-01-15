#include "freya_file.h"
#include "freya_logger.h"

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
  
  file_write_bytes(file, str.c_str(), str.size());
}

void file_write_bytes(File& file, const Transform& transform) {
  FREYA_DEBUG_ASSERT(file.is_open(), "Cannot perform an operation on an unopened file");
  
  f32 raw_data[] = {
    transform.position.x,  
    transform.position.y,  
    
    transform.scale.x,  
    transform.scale.y,  
    
    transform.rotation,  
  };

  file_write_bytes(file, raw_data, sizeof(raw_data));  
}

void file_write_bytes(File& file, const AudioSourceID& source) {
  FREYA_DEBUG_ASSERT(file.is_open(), "Cannot perform an operation on an unopened file");
 
  AudioSourceID source_id = source; 
  AudioSourceDesc desc    = audio_source_get_desc(source_id);

  f32 data[] = {
    desc.volume, 
    desc.pitch, 

    desc.position.x, 
    desc.position.y, 
    
    desc.velocity.x, 
    desc.velocity.y, 
    
    desc.direction.x, 
    desc.direction.y, 

    desc.is_looping,
  };

  file_write_bytes(file, data, sizeof(data));
}

void file_write_bytes(File& file, const AudioListenerDesc& listener_desc) {
  FREYA_DEBUG_ASSERT(file.is_open(), "Cannot perform an operation on an unopened file");
  
  f32 data[] = {
    listener_desc.volume, 

    listener_desc.position.x, 
    listener_desc.position.y, 
    
    listener_desc.velocity.x, 
    listener_desc.velocity.y, 
  };

  file_write_bytes(file, data, sizeof(data));
}

void file_write_bytes(File& file, const Timer& timer) {
  FREYA_DEBUG_ASSERT(file.is_open(), "Cannot perform an operation on an unopened file");
  
  file_write_bytes(file, &timer.limit, sizeof(timer.limit));
  file_write_bytes(file, &timer.is_one_shot, sizeof(timer.is_one_shot));
  file_write_bytes(file, &timer.is_active, sizeof(timer.is_active));
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
 
  // @TODO (File): We should NOT assign arbitrary sizes to the read string
  char c_str[1024];

  file_read_bytes(file, c_str, sizeof(c_str));
  *str = String(c_str);
}

void file_read_bytes(File& file, Transform* transform) {
  FREYA_DEBUG_ASSERT(file.is_open(), "Cannot perform an operation on an unopened file");
 
  f32 raw_data[5];
  file_read_bytes(file, raw_data, sizeof(raw_data));

  transform->position = Vec2(raw_data[0], raw_data[1]);
  transform->scale    = Vec2(raw_data[2], raw_data[3]);
  transform->rotation = raw_data[4];
  transform_apply(*transform); 
}

void file_read_bytes(File& file, AudioSourceID* source) {
  FREYA_DEBUG_ASSERT(file.is_open(), "Cannot perform an operation on an unopened file");
 
  f32 raw_data[9];
  file_read_bytes(file, raw_data, sizeof(raw_data));

  AudioSourceDesc desc {
    .volume = raw_data[0],
    .pitch  = raw_data[1],

    .position  = Vec2(raw_data[2], raw_data[3]),
    .velocity  = Vec2(raw_data[4], raw_data[5]),
    .direction = Vec2(raw_data[6], raw_data[7]),
    
    .is_looping = (bool)raw_data[8],
  };

  // Apply the new data
  
  audio_source_set_volume(*source, desc.volume);
  audio_source_set_pitch(*source, desc.pitch);

  audio_source_set_position(*source,desc.position);
  audio_source_set_velocity(*source, desc.velocity);
  audio_source_set_direction(*source, desc.direction);
  
  audio_source_set_looping(*source, desc.is_looping);
}

void file_read_bytes(File& file, AudioListenerDesc* listener) {
  FREYA_DEBUG_ASSERT(file.is_open(), "Cannot perform an operation on an unopened file");
 
  f32 raw_data[5];
  file_read_bytes(file, raw_data, sizeof(raw_data));

  listener->volume   = raw_data[0];
  listener->position = Vec2(raw_data[1], raw_data[2]),
  listener->velocity = Vec2(raw_data[3], raw_data[4]),
  
  audio_listener_set_volume(listener->volume);
  audio_listener_set_position(listener->position);
  audio_listener_set_velocity(listener->velocity);
}

void file_read_bytes(File& file, Timer* timer) {
  FREYA_DEBUG_ASSERT(file.is_open(), "Cannot perform an operation on an unopened file");

  file_read_bytes(file, &timer->limit, sizeof(timer->limit));
  file_read_bytes(file, &timer->is_one_shot, sizeof(timer->is_one_shot));
  file_read_bytes(file, &timer->is_active, sizeof(timer->is_active));
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

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

  // Write the size of the string

  u32 str_len = (u32)str.size();
  file_write_bytes(file, &str_len, sizeof(str_len));

  // Write the string itself (only if it's valid)
 
  if(str_len > 0) {
    file_write_bytes(file, str.data(), str_len);
  }
}

void file_write_bytes(File& file, const AudioBufferDesc& audio_desc) {
  FREYA_DEBUG_ASSERT(file.is_open(), "Cannot perform an operation on an unopened file");
  
  // Write the format
 
  u8 format = (u8)audio_desc.format;
  file_write_bytes(file, &format, sizeof(format));
 
  // Write the channels
  
  u8 channels = (u8)audio_desc.channels;
  file_write_bytes(file, &channels, sizeof(channels));

  // Write the sample rate
  file_write_bytes(file, &audio_desc.sample_rate, sizeof(audio_desc.sample_rate));
 
  // Write the samples

  u32 size = (u32)audio_desc.size;

  file_write_bytes(file, &size, sizeof(size));
  file_write_bytes(file, audio_desc.data, size);
}

void file_write_bytes(File& file, const GfxTextureDesc& tex_desc) {
  FREYA_DEBUG_ASSERT(file.is_open(), "Cannot perform an operation on an unopened file");

  // Write the texture's size
 
  u16 width  = (u16)tex_desc.width;
  u16 height = (u16)tex_desc.height;

  file_write_bytes(file, &width, sizeof(width));
  file_write_bytes(file, &height, sizeof(height));
 
  // Write the format

  u8 format = (u8)tex_desc.format;
  file_write_bytes(file, &format, sizeof(format));

  // Write the filter 
  
  u8 filter = (u8)tex_desc.filter;
  file_write_bytes(file, &filter, sizeof(filter));

  // Write the wrap mode
  
  u8 wrap = (u8)tex_desc.wrap_mode;
  file_write_bytes(file, &wrap, sizeof(wrap));

  // Write the data

  sizei pixel_size = (tex_desc.format == GFX_TEXTURE_FORMAT_RGBA16F) ? 4 : 1; 
  sizei data_size  = (width * height) * 4 * pixel_size; // 4 = color components

  file_write_bytes(file, tex_desc.data, data_size);
}

void file_write_bytes(File& file, const GfxShaderDesc& shader_desc) {
  FREYA_DEBUG_ASSERT(file.is_open(), "Cannot perform an operation on an unopened file");

  // Write the compue shader

  file_write_bytes(file, shader_desc.compute_source);
  if(!shader_desc.compute_source.empty()) {
    return;
  }

  // Write the vertex shader
  file_write_bytes(file, shader_desc.vertex_source);
  
  // Write the pixel shader
  file_write_bytes(file, shader_desc.pixel_source);
}

void file_read_bytes(File& file, AudioBufferDesc* out_desc) {
  FREYA_DEBUG_ASSERT(file.is_open(), "Cannot perform an operation on an unopened file");

  // Read the format
 
  u8 format;
  file_read_bytes(file, &format, sizeof(format));

  out_desc->format = (AudioBufferFormat)format;

  // Read the channels
  
  u8 channels;
  file_read_bytes(file, &channels, sizeof(channels));

  out_desc->channels = (u32)channels;

  // Read the sample rate
  file_read_bytes(file, &out_desc->sample_rate, sizeof(out_desc->sample_rate));
 
  // Read the samples

  u32 size;
  file_read_bytes(file, &size, sizeof(size));

  out_desc->size = (sizei)size;
  out_desc->data = memory_allocate(out_desc->size);

  file_read_bytes(file, out_desc->data, out_desc->size);
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

  // Read the length

  u32 str_len;
  file_read_bytes(file, &str_len, sizeof(str_len));
  
  // Read the string (only if it's valid)

  if(str_len > 0) {
    str->resize(str_len);
    file_read_bytes(file, str->data(), str_len);
  }
}

void file_read_bytes(File& file, GfxTextureDesc* out_desc) {
  FREYA_DEBUG_ASSERT(file.is_open(), "Cannot perform an operation on an unopened file");
  
  // Read the texture's size

  u16 width, height;
  file_read_bytes(file, &width, sizeof(width));  
  file_read_bytes(file, &height, sizeof(height));  
 
  out_desc->width  = width;
  out_desc->height = height;

  // Read the format

  u8 format;
  file_read_bytes(file, &format, sizeof(format));

  out_desc->format = (GfxTextureFormat)format;

  // Read the filter

  u8 filter;
  file_read_bytes(file, &filter, sizeof(filter));

  out_desc->filter = (GfxTextureFilter)filter;

  // Read the wrap

  u8 wrap;
  file_read_bytes(file, &wrap, sizeof(wrap));

  out_desc->wrap_mode = (GfxTextureWrap)wrap;

  // Read the data
  
  sizei pixel_size = (out_desc->format == GFX_TEXTURE_FORMAT_RGBA16F) ? 4 : 1; 
  sizei data_size  = (width * height) * 4 * pixel_size; // 4 = color components

  out_desc->data = memory_allocate(data_size);
  file_read_bytes(file, out_desc->data, data_size);
}

void file_read_bytes(File& file, GfxShaderDesc* out_desc) {
  FREYA_DEBUG_ASSERT(file.is_open(), "Cannot perform an operation on an unopened file");
  
  // Read the compue shader

  file_read_bytes(file, &out_desc->compute_source);
  if(!out_desc->compute_source.empty()) {
    return;
  }

  // Read the vertex shader
  file_read_bytes(file, &out_desc->vertex_source);
  
  // Read the pixel shader
  file_read_bytes(file, &out_desc->pixel_source);
}

void file_read_bytes(File& file, Transform* transform) {
  FREYA_DEBUG_ASSERT(file.is_open(), "Cannot perform an operation on an unopened file");
 
  f32 raw_data[5];
  file_read_bytes(file, raw_data, sizeof(raw_data));

  transform->position = Vec2(raw_data[0], raw_data[1]);
  transform->scale    = Vec2(raw_data[2], raw_data[3]);
  transform->rotation = raw_data[4];
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

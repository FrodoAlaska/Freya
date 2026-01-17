#include "asset_loaders.h"

#include "freya_assets.h"
#include "freya_logger.h"
#include "freya_memory.h"

#include <dr_libs/dr_mp3.h>
#include <dr_libs/dr_wav.h>
#include <stb/stb_vorbis.h>

/// ----------------------------------------------------------------------
/// Private functions

static bool mp3_convert(const freya::FilePath& path, freya::AudioBufferDesc* audio) {
  // Load the audio file

  drmp3_config config;
  drmp3_uint64 frames_count;
  
  audio->data = (void*)drmp3_open_file_and_read_pcm_frames_s16(path.c_str(), &config, &frames_count, nullptr);
  if (!audio->data) {
    FREYA_LOG_ERROR("Failed to read MP3 file at \'%s\'", path.c_str());
    return false;
  }
  
  // Convert to our audio format
  
  audio->format      = freya::AUDIO_BUFFER_FORMAT_I16; // @TODO (Loaders/audio): This might be different depending on the type
  audio->sample_rate = config.sampleRate;
  audio->channels    = config.channels;
  audio->size        = frames_count * (config.channels * sizeof(freya::i16)); 

  // Done!
  return true;
}

static bool wav_convert(const freya::FilePath& path, freya::AudioBufferDesc* audio) {
  // Load the audio file

  freya::u32 channels, sample_rate;
  drwav_uint64 frames_count;
  
  audio->data = (void*)drwav_open_file_and_read_pcm_frames_s16(path.c_str(), &channels, &sample_rate, &frames_count, nullptr);
  if (!audio->data) {
    FREYA_LOG_ERROR("Failed to read WAV file at \'%s\'", path.c_str());
    return false;
  }

  // Convert to our audio format
  
  audio->format      = freya::AUDIO_BUFFER_FORMAT_I16; // @TODO (Loaders/audio): This might be different depending on the type
  audio->sample_rate = sample_rate;
  audio->channels    = channels;
  audio->size        = frames_count * (channels * sizeof(freya::i16)); 

  // Done!
  return true;
}

static bool ogg_convert(const freya::FilePath& path, freya::AudioBufferDesc* audio) {
  // Open the OGG file and read the _whole_ file.
  
  freya::i32 channels, sample_rate;
  freya::i16* samples = nullptr;

  int frames  = stb_vorbis_decode_filename(path.c_str(), &channels, &sample_rate, &samples);
  audio->data = (void*)samples;

  if(frames == -1) {
    FREYA_LOG_ERROR("Failed to read OGG file at \'%s\'", path.c_str());
   
    freya::memory_free(audio->data);
    return false;
  } 
  
  // Convert to our audio format
  
  audio->format      = freya::AUDIO_BUFFER_FORMAT_I16; // @TODO (Loaders/audio): This might be different depending on the type
  audio->sample_rate = sample_rate;
  audio->channels    = channels;
  audio->size        = frames; 

  // Done!
  return true;
}

/// Private functions
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Audio loader functions

bool audio_loader_load(const freya::FilePath& path, freya::AudioBufferDesc* audio_desc) {
  // Convert the correct audio format

  freya::FilePath ext = freya::filepath_extension(path);
  bool has_loaded     = false;

  if(ext == ".mp3" || ext == ".MP3") {
    has_loaded = mp3_convert(path, audio_desc);
  }
  else if(ext == ".wav" || ext == ".WAV") {
    has_loaded = wav_convert(path, audio_desc);
  }
  else if(ext == ".ogg" || ext == ".OGG") {
    has_loaded = ogg_convert(path, audio_desc);
  }

  // Log depending on the outcome

  if(has_loaded) {
    FREYA_LOG_TRACE("Successfully loaded audio buffer at '\%s\'", path.c_str());
  } 
  else {
    FREYA_LOG_ERROR("Failed to load audio buffer at '\%s\'", path.c_str());
  }

  // Done!
  return has_loaded;
}

void audio_loader_unload(freya::AudioBufferDesc& audio_desc) {
  if(!audio_desc.data) {
    return;
  }

  freya::memory_free(audio_desc.data);
}

/// Audio loader functions
/// ----------------------------------------------------------------------

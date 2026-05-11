#pragma once

#include "freya_assets.h"

/// ----------------------------------------------------------------------
/// Texture loader functions

bool texture_loader_load(const freya::FilePath& path, freya::GfxTextureDesc* tex_desc);

void texture_loader_unload(freya::GfxTextureDesc& tex_desc);

/// Texture loader functions
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Shader loader functions

bool shader_loader_load(const freya::FilePath& path, freya::GfxShaderDesc* shader_desc);

/// Shader loader functions
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Font loader functions

bool font_loader_load(const freya::FilePath& path, freya::DynamicArray<freya::u8>& font_data);

/// Font loader functions
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Audio loader functions

bool audio_loader_load(const freya::FilePath& path, freya::AudioBufferDesc* audio_desc);

void audio_loader_unload(freya::AudioBufferDesc& audio_desc);

/// Audio loader functions
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// UIConfig loader functions

bool ui_config_loader_load(const freya::FilePath& path, freya::UIConfig* ui_cfg);

/// UIConfig loader functions
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// LUA state loader functions

bool lua_state_loader_load(const freya::FilePath& path, freya::String* out_src);

/// LUA state loader functions
/// ----------------------------------------------------------------------

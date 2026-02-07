#pragma once

#include "freya_gfx.h"
#include "freya_event.h"

#include <glad/glad.h>

///---------------------------------------------------------------------------------------------------------------------
/// Consts

const freya::sizei MAX_SHADER_LOG_MSG_LENGTH = 1024;

/// Consts
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// GL common functions

bool gl_on_framebuffer_resize_callback(const freya::Event& event, const void* dispatcher, const void* listener);

void gl_check_supported_version(const freya::i32 major, const freya::i32 minor);

GLenum gl_get_compare_func(const freya::GfxCompareFunc func);

GLenum gl_get_operation(const freya::GfxOperation op);

GLenum gl_get_blend_mode(const freya::GfxBlendMode mode);

GLenum gl_get_cull_order(const freya::GfxCullOrder order);

GLenum gl_get_cull_mode(const freya::GfxCullMode mode);

GLenum gl_get_buffer_type(const freya::GfxBufferType type);

GLenum gl_get_buffer_usage(const freya::GfxBufferUsage usage);

GLenum gl_get_draw_mode(const freya::GfxDrawMode mode);

freya::sizei gl_get_layout_size(const freya::GfxLayoutType layout);

GLenum gl_get_layout_type(const freya::GfxLayoutType layout);

freya::sizei gl_get_layout_count(const freya::GfxLayoutType layout);

void gl_get_texture_format(const freya::GfxTextureFormat format, GLenum* in_format, GLenum* gl_format, GLenum* gl_type);

void gl_get_texture_filter(const freya::GfxTextureFilter filter, GLenum* min, GLenum* mag);

GLenum gl_get_texture_wrap(const freya::GfxTextureWrap wrap);

GLenum gl_get_texture_access(const freya::GfxTextureAccess access);

GLenum gl_get_attachment_type(const freya::GfxTextureFormat format);

freya::u32 gl_get_clear_flags(const freya::u32 flags);

freya::GfxUniformType gl_get_uniform_type(const GLenum gl_type);

void gl_set_state(const freya::GfxStates state, const bool value);

void gl_set_depth_state(const freya::GfxDepthDesc& state);

void gl_set_stencil_state(const freya::GfxStencilDesc& state);

void gl_set_blend_state(const freya::GfxBlendDesc& state);

void gl_set_cull_state(const freya::GfxCullDesc& state);

void gl_check_shader_compile_error(const freya::u32 shader_id);

void gl_check_shader_linker_error(const freya::u32 shader_id);

void gl_update_texture_pixels(const freya::u32 texture_id, const freya::GfxTextureDesc& desc, GLenum gl_format, GLenum gl_pixel_type);

void gl_update_texture_storage(const freya::u32 texture_id, const freya::GfxTextureDesc& desc, GLenum in_format);

void gl_set_texture_pixel_align(const freya::GfxTextureFormat format);

/// GL common functions
///---------------------------------------------------------------------------------------------------------------------

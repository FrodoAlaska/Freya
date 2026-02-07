#include "gl_common.h"

#include "freya_logger.h"

///---------------------------------------------------------------------------------------------------------------------
/// Macros

#define SET_GFX_STATE(value, state) { \
  if(value) {                         \
    glEnable(state);                  \
  }                                   \
  else {                              \
    glDisable(state);                 \
  }                                   \
}

#define SET_BUFFER_BIT(value, bits, buffer) { \
  if(value) {                                 \
    SET_BIT(bits, buffer);                    \
  }                                           \
  else {                                      \
    UNSET_BIT(bits, buffer);                  \
  }                                           \
}

/// Macros
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// GL common functions

bool gl_on_framebuffer_resize_callback(const freya::Event& event, const void* dispatcher, const void* listener) {
  glViewport(0, 0, event.window_framebuffer_width, event.window_framebuffer_height);
  return true;
}

void gl_check_supported_version(const freya::i32 major, const freya::i32 minor) {
#if (FREYA_GFX_GL == 1)
  FREYA_ASSERT_LOG((major >= FREYA_GL_MINIMUM_MAJOR_VERSION) && (minor >= FREYA_GL_MINIMUM_MINOR_VERSION), 
                   "OpenGL versions less than 4.2 are not supported");
#elif (FREYA_GFX_GLES == 1)
  FREYA_ASSERT_LOG((major >= FREYA_GLES_MINIMUM_MAJOR_VERSION) && (minor >= FREYA_GLES_MINIMUM_MINOR_VERSION), 
                   "OpenGL-ES versions less than 3.0 are not supported");
#endif
}

GLenum gl_get_compare_func(const freya::GfxCompareFunc func) {
  switch(func) {
    case freya::GFX_COMPARE_ALWAYS:
      return GL_ALWAYS;
    case freya::GFX_COMPARE_NEVER:
      return GL_NEVER;
    case freya::GFX_COMPARE_EQUAL:
      return GL_EQUAL;
    case freya::GFX_COMPARE_LESS:
      return GL_LESS;
    case freya::GFX_COMPARE_LESS_EQUAL:
      return GL_LEQUAL;
    case freya::GFX_COMPARE_GREATER:
      return GL_GREATER;
    case freya::GFX_COMPARE_GREATER_EQUAL:
      return GL_GEQUAL;
    case freya::GFX_COMPARE_NOT_EQUAL:
      return GL_NOTEQUAL;
    default:
      return 0;
  } 
}

GLenum gl_get_operation(const freya::GfxOperation op) {
  switch(op) {
    case freya::GFX_OP_KEEP:
      return GL_KEEP;
    case freya::GFX_OP_ZERO:
      return GL_ZERO;
    case freya::GFX_OP_INVERT:
      return GL_INVERT;
    case freya::GFX_OP_REPLACE:
      return GL_REPLACE;
    case freya::GFX_OP_INCR:
      return GL_INCR;
    case freya::GFX_OP_DECR:
      return GL_DECR;
    case freya::GFX_OP_INCR_WRAP:
      return GL_INCR_WRAP;
    case freya::GFX_OP_DECR_WRAP:
      return GL_DECR_WRAP;
    default:
      return 0;
  }
}

GLenum gl_get_blend_mode(const freya::GfxBlendMode mode) {
  switch(mode) {
    case freya::GFX_BLEND_ZERO:
      return GL_ZERO;
    case freya::GFX_BLEND_ONE:
      return GL_ONE;
    case freya::GFX_BLEND_SRC_COLOR:
      return GL_SRC_COLOR;
    case freya::GFX_BLEND_DEST_COLOR:
      return GL_DST_COLOR;
    case freya::GFX_BLEND_SRC_ALPHA:
      return GL_SRC_ALPHA;
    case freya::GFX_BLEND_DEST_ALPHA:
      return GL_DST_ALPHA;
    case freya::GFX_BLEND_INV_SRC_COLOR:
      return GL_ONE_MINUS_SRC_COLOR;
    case freya::GFX_BLEND_INV_DEST_COLOR:
      return GL_ONE_MINUS_DST_COLOR;
    case freya::GFX_BLEND_INV_SRC_ALPHA:
      return GL_ONE_MINUS_SRC_ALPHA;
    case freya::GFX_BLEND_INV_DEST_ALPHA:
      return GL_ONE_MINUS_DST_ALPHA;
    case freya::GFX_BLEND_SRC_ALPHA_SATURATE:
      return GL_SRC_ALPHA_SATURATE;
    default:
      return 0;
  }
}

GLenum gl_get_cull_order(const freya::GfxCullOrder order) {
  switch(order) {
    case freya::GFX_ORDER_CLOCKWISE:
      return GL_CW;
    case freya::GFX_ORDER_COUNTER_CLOCKWISE:
      return GL_CCW;
    default:
      return 0;
  }
}

GLenum gl_get_cull_mode(const freya::GfxCullMode mode) {
  switch(mode) {
    case freya::GFX_CULL_FRONT:
      return GL_FRONT;
    case freya::GFX_CULL_BACK:
      return GL_BACK;
    case freya::GFX_CULL_FRONT_AND_BACK:
      return GL_FRONT_AND_BACK;
    default:
      return 0;
  }
}

GLenum gl_get_buffer_type(const freya::GfxBufferType type) {
  switch(type) {
    case freya::GFX_BUFFER_VERTEX:
      return GL_ARRAY_BUFFER;
    case freya::GFX_BUFFER_INDEX:
      return GL_ELEMENT_ARRAY_BUFFER;
    case freya::GFX_BUFFER_UNIFORM:
      return GL_UNIFORM_BUFFER;
    case freya::GFX_BUFFER_SHADER_STORAGE:
      return GL_SHADER_STORAGE_BUFFER;
    case freya::GFX_BUFFER_DRAW_INDIRECT:
      return GL_DRAW_INDIRECT_BUFFER;
  } 
}

GLenum gl_get_buffer_usage(const freya::GfxBufferUsage usage) {
  switch(usage) {
    case freya::GFX_BUFFER_USAGE_DYNAMIC_DRAW:
      return GL_DYNAMIC_DRAW;
    case freya::GFX_BUFFER_USAGE_DYNAMIC_READ:
      return GL_DYNAMIC_READ;
    case freya::GFX_BUFFER_USAGE_STATIC_DRAW:
      return GL_STATIC_DRAW;
    case freya::GFX_BUFFER_USAGE_STATIC_READ:
      return GL_STATIC_READ;
    default:
      return 0;
  }
}

GLenum gl_get_draw_mode(const freya::GfxDrawMode mode) {
  switch(mode) {
    case freya::GFX_DRAW_MODE_POINT:
      return GL_POINTS;
    case freya::GFX_DRAW_MODE_TRIANGLE:
      return GL_TRIANGLES;
    case freya::GFX_DRAW_MODE_TRIANGLE_STRIP:
      return GL_TRIANGLE_STRIP;
    case freya::GFX_DRAW_MODE_LINE:
      return GL_LINES;
    case freya::GFX_DRAW_MODE_LINE_STRIP:
      return GL_LINE_STRIP;
    default:
      return 0;
  }
}

freya::sizei gl_get_layout_size(const freya::GfxLayoutType layout) {
  switch(layout) {
    case freya::GFX_LAYOUT_FLOAT1:
      return sizeof(freya::f32);
    case freya::GFX_LAYOUT_FLOAT2:
      return sizeof(freya::f32) * 2;
    case freya::GFX_LAYOUT_FLOAT3:
      return sizeof(freya::f32) * 3;
    case freya::GFX_LAYOUT_FLOAT4:
      return sizeof(freya::f32) * 4;

    case freya::GFX_LAYOUT_BYTE1:
    case freya::GFX_LAYOUT_UBYTE1:
      return sizeof(freya::i8);
    case freya::GFX_LAYOUT_BYTE2:
    case freya::GFX_LAYOUT_UBYTE2:
      return sizeof(freya::i8) * 2;
    case freya::GFX_LAYOUT_BYTE3:
    case freya::GFX_LAYOUT_UBYTE3:
      return sizeof(freya::i8) * 3;
    case freya::GFX_LAYOUT_BYTE4:
    case freya::GFX_LAYOUT_UBYTE4:
      return sizeof(freya::i8) * 4;

    case freya::GFX_LAYOUT_SHORT1:
    case freya::GFX_LAYOUT_USHORT1:
      return sizeof(freya::i16);
    case freya::GFX_LAYOUT_SHORT2:
    case freya::GFX_LAYOUT_USHORT2:
      return sizeof(freya::i16) * 2;
    case freya::GFX_LAYOUT_SHORT3:
    case freya::GFX_LAYOUT_USHORT3:
      return sizeof(freya::i16) * 3;
    case freya::GFX_LAYOUT_SHORT4:
    case freya::GFX_LAYOUT_USHORT4:
      return sizeof(freya::i16) * 4;

    case freya::GFX_LAYOUT_INT1:
    case freya::GFX_LAYOUT_UINT1:
      return sizeof(freya::i32);
    case freya::GFX_LAYOUT_INT2:
    case freya::GFX_LAYOUT_UINT2:
      return sizeof(freya::i32) * 2;
    case freya::GFX_LAYOUT_INT3:
    case freya::GFX_LAYOUT_UINT3:
      return sizeof(freya::i32) * 3;
    case freya::GFX_LAYOUT_INT4:
    case freya::GFX_LAYOUT_UINT4:
      return sizeof(freya::i32) * 4;

    default: 
      return 0;
  }
}

GLenum gl_get_layout_type(const freya::GfxLayoutType layout) {
  switch(layout) {
    case freya::GFX_LAYOUT_FLOAT1:
    case freya::GFX_LAYOUT_FLOAT2:
    case freya::GFX_LAYOUT_FLOAT3:
    case freya::GFX_LAYOUT_FLOAT4:
    case freya::GFX_LAYOUT_MAT3:
    case freya::GFX_LAYOUT_MAT4:
      return GL_FLOAT;
    case freya::GFX_LAYOUT_BYTE1:
    case freya::GFX_LAYOUT_BYTE2:
    case freya::GFX_LAYOUT_BYTE3:
    case freya::GFX_LAYOUT_BYTE4:
      return GL_BYTE;
    case freya::GFX_LAYOUT_UBYTE1:
    case freya::GFX_LAYOUT_UBYTE2:
    case freya::GFX_LAYOUT_UBYTE3:
    case freya::GFX_LAYOUT_UBYTE4:
      return GL_UNSIGNED_BYTE;
    case freya::GFX_LAYOUT_SHORT1:
    case freya::GFX_LAYOUT_SHORT2:
    case freya::GFX_LAYOUT_SHORT3:
    case freya::GFX_LAYOUT_SHORT4:
      return GL_SHORT;
    case freya::GFX_LAYOUT_USHORT1:
    case freya::GFX_LAYOUT_USHORT2:
    case freya::GFX_LAYOUT_USHORT3:
    case freya::GFX_LAYOUT_USHORT4:
      return GL_UNSIGNED_SHORT;
    case freya::GFX_LAYOUT_INT1:
    case freya::GFX_LAYOUT_INT2:
    case freya::GFX_LAYOUT_INT3:
    case freya::GFX_LAYOUT_INT4:
      return GL_INT;
    case freya::GFX_LAYOUT_UINT1:
    case freya::GFX_LAYOUT_UINT2:
    case freya::GFX_LAYOUT_UINT3:
    case freya::GFX_LAYOUT_UINT4:
      return GL_UNSIGNED_INT;
    default:
      return 0;
  }
}

freya::sizei gl_get_layout_count(const freya::GfxLayoutType layout) {
  switch(layout) {
    case freya::GFX_LAYOUT_FLOAT1:
    case freya::GFX_LAYOUT_BYTE1:
    case freya::GFX_LAYOUT_UBYTE1:
    case freya::GFX_LAYOUT_SHORT1:
    case freya::GFX_LAYOUT_USHORT1:
    case freya::GFX_LAYOUT_INT1:
    case freya::GFX_LAYOUT_UINT1:
      return 1;
    case freya::GFX_LAYOUT_FLOAT2:
    case freya::GFX_LAYOUT_BYTE2:
    case freya::GFX_LAYOUT_UBYTE2:
    case freya::GFX_LAYOUT_SHORT2:
    case freya::GFX_LAYOUT_USHORT2:
    case freya::GFX_LAYOUT_INT2:
    case freya::GFX_LAYOUT_UINT2:
      return 2;
    case freya::GFX_LAYOUT_FLOAT3:
    case freya::GFX_LAYOUT_BYTE3:
    case freya::GFX_LAYOUT_UBYTE3:
    case freya::GFX_LAYOUT_SHORT3:
    case freya::GFX_LAYOUT_USHORT3:
    case freya::GFX_LAYOUT_INT3:
    case freya::GFX_LAYOUT_UINT3:
      return 3;
    case freya::GFX_LAYOUT_FLOAT4:
    case freya::GFX_LAYOUT_BYTE4:
    case freya::GFX_LAYOUT_UBYTE4:
    case freya::GFX_LAYOUT_SHORT4:
    case freya::GFX_LAYOUT_USHORT4:
    case freya::GFX_LAYOUT_INT4:
    case freya::GFX_LAYOUT_UINT4:
      return 4;
    default:
      return 0;
  }
}

void gl_get_texture_format(const freya::GfxTextureFormat format, GLenum* in_format, GLenum* gl_format, GLenum* gl_type) {
  switch(format) {
    case freya::GFX_TEXTURE_FORMAT_R8:
      *in_format = GL_R8;
      *gl_format = GL_RED;
      *gl_type   = GL_UNSIGNED_BYTE;
      break;
    case freya::GFX_TEXTURE_FORMAT_R16:
      *in_format = GL_R16;
      *gl_format = GL_RED;
      *gl_type   = GL_UNSIGNED_SHORT;
      break;
    case freya::GFX_TEXTURE_FORMAT_R16F:
      *in_format = GL_R16F;
      *gl_format = GL_RED;
      *gl_type   = GL_FLOAT;
      break;
    case freya::GFX_TEXTURE_FORMAT_R32F:
      *in_format = GL_R32F;
      *gl_format = GL_RED;
      *gl_type   = GL_FLOAT;
      break;
    case freya::GFX_TEXTURE_FORMAT_RG8:
      *in_format = GL_RG8;
      *gl_format = GL_RG;
      *gl_type   = GL_UNSIGNED_BYTE;
      break;
    case freya::GFX_TEXTURE_FORMAT_RG16:
      *in_format = GL_RG16;
      *gl_format = GL_RG;
      *gl_type   = GL_UNSIGNED_SHORT;
      break;
    case freya::GFX_TEXTURE_FORMAT_RG16F:
      *in_format = GL_RG16F;
      *gl_format = GL_RG;
      *gl_type   = GL_FLOAT;
      break;
    case freya::GFX_TEXTURE_FORMAT_RG32F:
      *in_format = GL_RG32F;
      *gl_format = GL_RG;
      *gl_type   = GL_FLOAT;
      break;
    case freya::GFX_TEXTURE_FORMAT_RGBA8:
      *in_format = GL_RGBA8;
      *gl_format = GL_RGBA;
      *gl_type   = GL_UNSIGNED_BYTE;
      break;
    case freya::GFX_TEXTURE_FORMAT_RGBA16:
      *in_format = GL_RGBA16;
      *gl_format = GL_RGBA;
      *gl_type   = GL_UNSIGNED_SHORT;
      break;
    case freya::GFX_TEXTURE_FORMAT_RGBA16F:
      *in_format = GL_RGBA16F;
      *gl_format = GL_RGBA;
      *gl_type   = GL_FLOAT;
      break;
    case freya::GFX_TEXTURE_FORMAT_RGBA32F:
      *in_format = GL_RGBA32F;
      *gl_format = GL_RGBA;
      *gl_type   = GL_FLOAT;
      break;
    case freya::GFX_TEXTURE_FORMAT_DEPTH16:
      *in_format = GL_DEPTH_COMPONENT16;
      *gl_format = GL_DEPTH_COMPONENT;
      *gl_type   = GL_UNSIGNED_SHORT;
    case freya::GFX_TEXTURE_FORMAT_DEPTH24:
      *in_format = GL_DEPTH_COMPONENT24;
      *gl_format = GL_DEPTH_COMPONENT;
      *gl_type   = GL_UNSIGNED_INT;
    case freya::GFX_TEXTURE_FORMAT_DEPTH32F:
      *in_format = GL_DEPTH_COMPONENT32F;
      *gl_format = GL_DEPTH_COMPONENT;
      *gl_type   = GL_FLOAT;
    case freya::GFX_TEXTURE_FORMAT_STENCIL8:
      *in_format = GL_STENCIL_INDEX8;
      *gl_format = GL_STENCIL_INDEX;
      *gl_type   = GL_UNSIGNED_BYTE;
    case freya::GFX_TEXTURE_FORMAT_DEPTH_STENCIL_24_8:
      *in_format = GL_DEPTH24_STENCIL8;
      *gl_format = GL_DEPTH_STENCIL;
      *gl_type   = GL_UNSIGNED_INT_24_8;
      break;
    default:
      break;
  }
}

void gl_get_texture_filter(const freya::GfxTextureFilter filter, GLenum* min, GLenum* mag) {
  switch(filter) {
    case freya::GFX_TEXTURE_FILTER_MIN_MAG_LINEAR:
      *min = GL_LINEAR; 
      *mag = GL_LINEAR;
      break;
    case freya::GFX_TEXTURE_FILTER_MIN_MAG_NEAREST:
      *min = GL_NEAREST; 
      *mag = GL_NEAREST;
      break;
    case freya::GFX_TEXTURE_FILTER_MIN_LINEAR_MAG_NEAREST:
      *min = GL_LINEAR; 
      *mag = GL_NEAREST;
      break;
    case freya::GFX_TEXTURE_FILTER_MIN_NEAREST_MAG_LINEAR:
      *min = GL_NEAREST; 
      *mag = GL_LINEAR;
      break;
    case freya::GFX_TEXTURE_FILTER_MIN_TRILINEAR_MAG_LINEAR:
      *min = GL_LINEAR_MIPMAP_LINEAR; 
      *mag = GL_LINEAR;
      break;
    case freya::GFX_TEXTURE_FILTER_MIN_TRILINEAR_MAG_NEAREST:
      *min = GL_LINEAR_MIPMAP_LINEAR; 
      *mag = GL_NEAREST;
      break;
    default:
      break;
  }
}

GLenum gl_get_texture_wrap(const freya::GfxTextureWrap wrap) {
  switch(wrap) {
    case freya::GFX_TEXTURE_WRAP_REPEAT: 
      return GL_REPEAT;
    case freya::GFX_TEXTURE_WRAP_MIRROR: 
      return GL_MIRRORED_REPEAT;
    case freya::GFX_TEXTURE_WRAP_CLAMP: 
      return GL_CLAMP_TO_EDGE;
    case freya::GFX_TEXTURE_WRAP_BORDER_COLOR:
      return GL_CLAMP_TO_BORDER;
    default:
      return 0;
  }
}

GLenum gl_get_texture_access(const freya::GfxTextureAccess access) {
  switch(access) {
    case freya::GFX_TEXTURE_ACCESS_READ: 
      return GL_READ_ONLY;
    case freya::GFX_TEXTURE_ACCESS_WRITE: 
      return GL_WRITE_ONLY;
    case freya::GFX_TEXTURE_ACCESS_READ_WRITE: 
      return GL_READ_WRITE;
    default:
      return 0;
  }
}

GLenum gl_get_attachment_type(const freya::GfxTextureFormat format) {
  switch(format) {
    case freya::GFX_TEXTURE_FORMAT_DEPTH16:
    case freya::GFX_TEXTURE_FORMAT_DEPTH24:
    case freya::GFX_TEXTURE_FORMAT_DEPTH32F:
    case freya::GFX_TEXTURE_FORMAT_DEPTH_STENCIL_24_8:
      return GL_DEPTH_ATTACHMENT;
    case freya::GFX_TEXTURE_FORMAT_STENCIL8:
      return GL_STENCIL_ATTACHMENT;
    default:
      return GL_TEXTURE_2D;
  }
}

freya::GfxUniformType gl_get_uniform_type(const GLenum gl_type) {
  switch(gl_type) {
    case GL_FLOAT: 
      return freya::GFX_UNIFORM_FLOAT1;
    case GL_FLOAT_VEC2: 
      return freya::GFX_UNIFORM_FLOAT2;
    case GL_FLOAT_VEC3: 
      return freya::GFX_UNIFORM_FLOAT3;
    case GL_FLOAT_VEC4: 
      return freya::GFX_UNIFORM_FLOAT4;
    
    case GL_INT: 
      return freya::GFX_UNIFORM_INT1;
    case GL_INT_VEC2: 
      return freya::GFX_UNIFORM_INT2;
    case GL_INT_VEC3: 
      return freya::GFX_UNIFORM_INT3;
    case GL_INT_VEC4: 
      return freya::GFX_UNIFORM_INT4;
    
    case GL_UNSIGNED_INT: 
      return freya::GFX_UNIFORM_UINT1;
    case GL_UNSIGNED_INT_VEC2: 
      return freya::GFX_UNIFORM_UINT2;
    case GL_UNSIGNED_INT_VEC3: 
      return freya::GFX_UNIFORM_UINT3;
    case GL_UNSIGNED_INT_VEC4: 
      return freya::GFX_UNIFORM_UINT4;
    
    case GL_BOOL: 
      return freya::GFX_UNIFORM_BOOL1;
    case GL_BOOL_VEC2: 
      return freya::GFX_UNIFORM_BOOL2;
    case GL_BOOL_VEC3: 
      return freya::GFX_UNIFORM_BOOL3;
    case GL_BOOL_VEC4: 
      return freya::GFX_UNIFORM_BOOL4;
    
    case GL_FLOAT_MAT2: 
      return freya::GFX_UNIFORM_MAT2;
    case GL_FLOAT_MAT3: 
      return freya::GFX_UNIFORM_MAT3;
    case GL_FLOAT_MAT4: 
      return freya::GFX_UNIFORM_MAT4;
    case GL_FLOAT_MAT2x3: 
      return freya::GFX_UNIFORM_MAT2X3;
    case GL_FLOAT_MAT2x4: 
      return freya::GFX_UNIFORM_MAT2X4;
    case GL_FLOAT_MAT3x2: 
      return freya::GFX_UNIFORM_MAT3X2;
    case GL_FLOAT_MAT3x4: 
      return freya::GFX_UNIFORM_MAT3X4;
    case GL_FLOAT_MAT4x2: 
      return freya::GFX_UNIFORM_MAT4X2;
    case GL_FLOAT_MAT4x3: 
      return freya::GFX_UNIFORM_MAT4X3;
    
    case GL_SAMPLER_1D: 
      return freya::GFX_UNIFORM_SAMPLER_1D;
    case GL_SAMPLER_2D: 
      return freya::GFX_UNIFORM_SAMPLER_2D;
    case GL_SAMPLER_3D: 
      return freya::GFX_UNIFORM_SAMPLER_3D;
    case GL_SAMPLER_CUBE: 
      return freya::GFX_UNIFORM_SAMPLER_CUBE;
    
    case GL_SAMPLER_1D_SHADOW: 
      return freya::GFX_UNIFORM_SAMPLER_1D_SHADOW;
    case GL_SAMPLER_2D_SHADOW: 
      return freya::GFX_UNIFORM_SAMPLER_2D_SHADOW;
    case GL_SAMPLER_CUBE_SHADOW: 
      return freya::GFX_UNIFORM_SAMPLER_CUBE_SHADOW;
    
    case GL_SAMPLER_1D_ARRAY: 
      return freya::GFX_UNIFORM_SAMPLER_1D_ARRAY;
    case GL_SAMPLER_2D_ARRAY: 
      return freya::GFX_UNIFORM_SAMPLER_2D_ARRAY;
    case GL_SAMPLER_1D_ARRAY_SHADOW: 
      return freya::GFX_UNIFORM_SAMPLER_1D_ARRAY_SHADOW;
    case GL_SAMPLER_2D_ARRAY_SHADOW: 
      return freya::GFX_UNIFORM_SAMPLER_2D_ARRAY_SHADOW;
    
    case GL_IMAGE_1D: 
      return freya::GFX_UNIFORM_IMAGE_1D;
    case GL_IMAGE_2D: 
      return freya::GFX_UNIFORM_IMAGE_2D;
    case GL_IMAGE_3D: 
      return freya::GFX_UNIFORM_IMAGE_3D;
    case GL_IMAGE_CUBE: 
      return freya::GFX_UNIFORM_IMAGE_CUBE;
    
    case GL_IMAGE_1D_ARRAY: 
      return freya::GFX_UNIFORM_IMAGE_1D_ARRAY;
    case GL_IMAGE_2D_ARRAY: 
      return freya::GFX_UNIFORM_IMAGE_2D_ARRAY;

    default: 
      return (freya::GfxUniformType)-1;
  }
}

freya::u32 gl_get_clear_flags(const freya::u32 flags) {
  freya::u32 gl_flags = 0;
  
  if(IS_BIT_SET(flags, freya::GFX_CLEAR_FLAGS_NONE)) {
    return 0;
  }

  if(IS_BIT_SET(flags, freya::GFX_CLEAR_FLAGS_COLOR_BUFFER)) {
    gl_flags |= GL_COLOR_BUFFER_BIT;    
  }
  
  if(IS_BIT_SET(flags, freya::GFX_CLEAR_FLAGS_DEPTH_BUFFER)) {
    gl_flags |= GL_DEPTH_BUFFER_BIT;    
  }
  
  if(IS_BIT_SET(flags, freya::GFX_CLEAR_FLAGS_STENCIL_BUFFER)) {
    gl_flags |= GL_STENCIL_BUFFER_BIT;    
  }

  return gl_flags;
}

void gl_set_state(const freya::GfxStates state, const bool value) {
  switch(state) {
    case freya::GFX_STATE_DEPTH:
      SET_GFX_STATE(value, GL_DEPTH_TEST);
      break;
    case freya::GFX_STATE_STENCIL:
      SET_GFX_STATE(value, GL_STENCIL_TEST);
      break;
    case freya::GFX_STATE_BLEND:
      SET_GFX_STATE(value, GL_BLEND);
      break;
    case freya::GFX_STATE_MSAA:
      SET_GFX_STATE(value, GL_MULTISAMPLE);
      break;
    case freya::GFX_STATE_CULL:
      SET_GFX_STATE(value, GL_CULL_FACE);
      break;
    case freya::GFX_STATE_SCISSOR:
      SET_GFX_STATE(value, GL_SCISSOR_TEST);
      break;
  }
}

void gl_set_depth_state(const freya::GfxDepthDesc& state) {
  GLenum func = gl_get_compare_func(state.compare_func);

  glDepthFunc(func);
  glDepthMask(state.depth_write_enabled);
}

void gl_set_stencil_state(const freya::GfxStencilDesc& state) {
  GLenum func  = gl_get_compare_func(state.compare_func);
  GLenum face  = gl_get_cull_mode(state.polygon_face); 
  GLenum sfail = gl_get_operation(state.stencil_fail_op); 
  GLenum dfail = gl_get_operation(state.depth_fail_op); 
  GLenum dpass = gl_get_operation(state.depth_pass_op); 

  glStencilFuncSeparate(face, func, state.ref, state.mask);
  glStencilOpSeparate(face, sfail, dfail, dpass);
  glStencilMaskSeparate(face, state.mask);
}

void gl_set_blend_state(const freya::GfxBlendDesc& state) {
  GLenum src_color = gl_get_blend_mode(state.src_color_blend);
  GLenum dst_color = gl_get_blend_mode(state.dest_color_blend);

  GLenum src_alpha = gl_get_blend_mode(state.src_alpha_blend);
  GLenum dst_alpha = gl_get_blend_mode(state.dest_alpha_blend);

  const freya::f32* factor = state.blend_factor;
  
  glBlendFuncSeparate(src_color, dst_color, src_alpha, dst_alpha);
  glBlendColor(factor[0], factor[1], factor[2], factor[3]);
}

void gl_set_cull_state(const freya::GfxCullDesc& state) {
  GLenum front_face = gl_get_cull_order(state.front_face);
  GLenum face       = gl_get_cull_mode(state.cull_mode);
  
  glCullFace(face);
  glFrontFace(front_face);
}

void gl_check_shader_compile_error(const freya::u32 shader_id) {
  freya::i32 success;
  freya::i8 log_info[MAX_SHADER_LOG_MSG_LENGTH];

  glGetShaderiv(shader_id, GL_COMPILE_STATUS, &success); 

  if(!success) {
    glGetShaderInfoLog(shader_id, MAX_SHADER_LOG_MSG_LENGTH, nullptr, log_info);
    FREYA_LOG_WARN("SHADER-WARNING: %s", log_info);
  }
}

void gl_check_shader_linker_error(const freya::u32 shader_id) {
  freya::i32 success;
  freya::i8 log_info[MAX_SHADER_LOG_MSG_LENGTH];

  glGetProgramiv(shader_id, GL_LINK_STATUS, &success); 

  if(!success) {
    glGetProgramInfoLog(shader_id, MAX_SHADER_LOG_MSG_LENGTH, nullptr, log_info);
    FREYA_LOG_WARN("SHADER-WARNING: %s", log_info);
  }
}

void gl_update_texture_pixels(const freya::u32 texture_id, const freya::GfxTextureDesc& desc, GLenum gl_format, GLenum gl_pixel_type) {
  switch(desc.type) {
    case freya::GFX_TEXTURE_1D: 
    case freya::GFX_TEXTURE_IMAGE_1D:
      glTextureSubImage1D(texture_id, 
                          desc.mips, 
                          0, 
                          desc.width, 
                          gl_format, 
                          gl_pixel_type, 
                          desc.data);
      break;
    case freya::GFX_TEXTURE_2D:
    case freya::GFX_TEXTURE_2D_PROXY:
    case freya::GFX_TEXTURE_IMAGE_2D:
    case freya::GFX_TEXTURE_1D_ARRAY:
    case freya::GFX_TEXTURE_1D_ARRAY_PROXY:
      glTextureSubImage2D(texture_id, 
                          0, 
                          0, 0,
                          desc.width, desc.height,
                          gl_format, 
                          gl_pixel_type, 
                          desc.data);
      break;
    case freya::GFX_TEXTURE_3D:
    case freya::GFX_TEXTURE_IMAGE_3D:
    case freya::GFX_TEXTURE_2D_ARRAY:
      glTextureSubImage3D(texture_id, 
                          0, 
                          0, 0, 0,
                          desc.width, desc.height, desc.depth,
                          gl_format, 
                          gl_pixel_type, 
                          desc.data);
      break;
    case freya::GFX_TEXTURE_DEPTH_TARGET:
    case freya::GFX_TEXTURE_STENCIL_TARGET:
    case freya::GFX_TEXTURE_DEPTH_STENCIL_TARGET:
      break;
    default:
      break;
  }
}

void gl_update_texture_storage(const freya::u32 texture_id, const freya::GfxTextureDesc& desc, GLenum in_format) {
  switch(desc.type) {
    case freya::GFX_TEXTURE_1D: 
    case freya::GFX_TEXTURE_IMAGE_1D:
      glTextureStorage1D(texture_id, desc.mips, in_format, desc.width);
      break;
    case freya::GFX_TEXTURE_2D:
    case freya::GFX_TEXTURE_2D_PROXY:
    case freya::GFX_TEXTURE_IMAGE_2D:
    case freya::GFX_TEXTURE_1D_ARRAY_PROXY:
      glTextureStorage2D(texture_id, desc.mips, in_format, desc.width, desc.height);
      break;
    case freya::GFX_TEXTURE_3D:
    case freya::GFX_TEXTURE_IMAGE_3D:
    case freya::GFX_TEXTURE_2D_ARRAY:
      glTextureStorage3D(texture_id, desc.mips, in_format, desc.width, desc.height, desc.depth);
      break;
    case freya::GFX_TEXTURE_DEPTH_TARGET:
    case freya::GFX_TEXTURE_STENCIL_TARGET:
    case freya::GFX_TEXTURE_DEPTH_STENCIL_TARGET:
      glNamedRenderbufferStorage(texture_id, in_format, desc.width, desc.height);
      break;
    default:
      break;
  }
}

void gl_set_texture_pixel_align(const freya::GfxTextureFormat format) {
  switch(format) {
    case freya::GFX_TEXTURE_FORMAT_R8:
    case freya::GFX_TEXTURE_FORMAT_R16:
    case freya::GFX_TEXTURE_FORMAT_R16F:
    case freya::GFX_TEXTURE_FORMAT_R32F:
      glPixelStorei(GL_PACK_ALIGNMENT, 1);
      glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
      break;
    case freya::GFX_TEXTURE_FORMAT_RG8:
    case freya::GFX_TEXTURE_FORMAT_RG16:
    case freya::GFX_TEXTURE_FORMAT_RG16F:
    case freya::GFX_TEXTURE_FORMAT_RG32F:
      glPixelStorei(GL_PACK_ALIGNMENT, 2);
      glPixelStorei(GL_UNPACK_ALIGNMENT, 2);
      break;
    case freya::GFX_TEXTURE_FORMAT_RGBA8:
    case freya::GFX_TEXTURE_FORMAT_RGBA16:
    case freya::GFX_TEXTURE_FORMAT_RGBA16F:
    case freya::GFX_TEXTURE_FORMAT_RGBA32F:
      glPixelStorei(GL_PACK_ALIGNMENT, 4);
      glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
      break;
    default:
      break;
  }
}

/// GL common functions
///---------------------------------------------------------------------------------------------------------------------

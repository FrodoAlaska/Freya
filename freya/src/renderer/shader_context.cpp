#include "freya_render.h"
#include "freya_logger.h"

//////////////////////////////////////////////////////////////////////////

namespace freya { // Start of freya 

///---------------------------------------------------------------------------------------------------------------------
/// Private functions

static void cache_uniform(ShaderContext* ctx, const String& name) {
  GfxShader* shader = ctx->shader; 
  
  // Get the new uniform location, first
  i32 location = gfx_shader_uniform_lookup(shader, name.c_str());
  
  // The uniform just does not exist in the shader at all 
  
  if(location == -1) {
    FREYA_LOG_WARN("Could not find uniform \'%s\' in ShaderContext", name.c_str());
    return;
  }

  // New uniform!
  
  ctx->uniforms_cache[name] = location; 
  FREYA_LOG_DEBUG("Cache uniform \'%s\' with location \'%i\' in ShaderContext...", name.c_str(), location);
}

static void check_and_send_uniform(ShaderContext* ctx, const String& name, GfxLayoutType type, const void* data) {
  GfxShader* shader = ctx->shader; 

  // Send the uniform (only if it is valid)
  
  if(ctx->uniforms_cache.find(name) != ctx->uniforms_cache.end()) {
    gfx_shader_upload_uniform(shader, ctx->uniforms_cache[name], type, data);
    return;
  }
  
  // @TODO (ShaderContext): Silent error??
}

static void check_and_send_uniform_array(ShaderContext* ctx, const String& name, GfxLayoutType type, const void* data, const sizei count) {
  GfxShader* shader = ctx->shader; 

  // Send the uniform (only if it is valid)
  
  if(ctx->uniforms_cache.find(name) != ctx->uniforms_cache.end()) {
    gfx_shader_upload_uniform_array(shader, ctx->uniforms_cache[name], type, data, count);
    return;
  }
  
  // @TODO (ShaderContext): Silent error??
}

/// Private functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// ShaderContext functions

void shader_context_cache_uniform(ShaderContext* ctx, const String& uniform_name) {
  FREYA_ASSERT(ctx);
  FREYA_ASSERT(ctx->shader)
  
  cache_uniform(ctx, uniform_name);
}

void shader_context_set_uniform(ShaderContext* ctx, const String& uniform_name, const i32 value) {
  FREYA_ASSERT(ctx);
  FREYA_ASSERT(ctx->shader)

  check_and_send_uniform(ctx, uniform_name, GFX_LAYOUT_INT1, &value);
}

void shader_context_set_uniform(ShaderContext* ctx, const String& uniform_name, const f32 value) {
  FREYA_ASSERT(ctx);
  FREYA_ASSERT(ctx->shader)

  check_and_send_uniform(ctx, uniform_name, GFX_LAYOUT_FLOAT1, &value);
}

void shader_context_set_uniform(ShaderContext* ctx, const String& uniform_name, const Vec2& value) {
  FREYA_ASSERT(ctx);
  FREYA_ASSERT(ctx->shader)

  check_and_send_uniform(ctx, uniform_name, GFX_LAYOUT_FLOAT2, &value);
}

void shader_context_set_uniform(ShaderContext* ctx, const String& uniform_name, const Vec3& value) {
  FREYA_ASSERT(ctx);
  FREYA_ASSERT(ctx->shader)

  check_and_send_uniform(ctx, uniform_name, GFX_LAYOUT_FLOAT3, &value);
}

void shader_context_set_uniform(ShaderContext* ctx, const String& uniform_name, const Vec4& value) {
  FREYA_ASSERT(ctx);
  FREYA_ASSERT(ctx->shader)

  check_and_send_uniform(ctx, uniform_name, GFX_LAYOUT_FLOAT4, &value);
}

void shader_context_set_uniform(ShaderContext* ctx, const String& uniform_name, const Mat4& value) {
  FREYA_ASSERT(ctx);
  FREYA_ASSERT(ctx->shader)
  
  check_and_send_uniform(ctx, uniform_name, GFX_LAYOUT_MAT4, &value);
}

void shader_context_set_uniform_array(ShaderContext* ctx, const String& uniform_name, const i32* values, const sizei count) {
  FREYA_ASSERT(ctx);
  FREYA_ASSERT(ctx->shader)

  check_and_send_uniform_array(ctx, uniform_name, GFX_LAYOUT_INT1, values, count);
}

void shader_context_set_uniform_array(ShaderContext* ctx, const String& uniform_name, const f32* values, const sizei count) {
  FREYA_ASSERT(ctx);
  FREYA_ASSERT(ctx->shader)
  
  check_and_send_uniform_array(ctx, uniform_name, GFX_LAYOUT_FLOAT1, values, count);
}

/// ShaderContext functions
///---------------------------------------------------------------------------------------------------------------------

} // End of freya

//////////////////////////////////////////////////////////////////////////

#include "freya_gfx.h"
#include "freya_event.h"
#include "freya_memory.h"
#include "freya_logger.h"

#if (FREYA_GFX_GLES == 1) // FREYA_GFX_GLES

//////////////////////////////////////////////////////////////////////////

#include <glad/glad.h>

#include <cstring>

namespace freya { // Start of freya

///---------------------------------------------------------------------------------------------------------------------
/// GfxContext
struct GfxContext {
  GfxContextDesc desc = {};
  GfxStates states;

  u32 current_target = 0; 

  u32 default_clear_flags = 0;
  u32 current_clear_flags = 0;

  GfxPipeline* bound_pipeline = nullptr;
  DynamicArray<String> extensions;
};
/// GfxContext
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// GfxFramebuffer
struct GfxFramebuffer {
  GfxFramebufferDesc desc = {};
  
  u32 clear_flags;
  u32 id;

  u32 color_textures[FRAMEBUFFER_ATTACHMENTS_MAX] = {GL_NONE, GL_NONE, GL_NONE, GL_NONE};
  u32 depth_texture   = GL_NONE; 
  u32 stencil_texture = GL_NONE;
};
/// GfxFramebuffer
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// GfxBuffer  
struct GfxBuffer {
  GfxBufferDesc desc = {};
  GfxContext* gfx    = nullptr;

  u32 id;

  GLenum gl_buff_type; 
  GLenum gl_buff_usage;
};
/// GfxBuffer  
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// GfxShader
struct GfxShader {
  GfxContext* gfx    = nullptr;
  GfxShaderDesc desc = {};

  u32 id; 
  u32 vert_id, frag_id, compute_id;
};
/// GfxShader
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// GfxTexture
struct GfxTexture {
  GfxTextureDesc desc = {};
  GfxContext* gfx     = nullptr;

  u32 id;
  GLuint64 bindless_id;

  GLenum gl_type;
};
/// GfxTexture
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// GfxCubemap
struct GfxCubemap {
  GfxCubemapDesc desc = {};
  GfxContext* gfx     = nullptr;
  
  u32 id;
};
/// GfxCubemap
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// GfxPipeline
struct GfxPipeline {
  GfxPipelineDesc desc = {};
  GfxContext* gfx      = nullptr;

  u32 vertex_array;

  GfxBuffer* vertex_buffer = nullptr;
  sizei vertex_count       = 0;

  GfxBuffer* index_buffer = nullptr; 
  sizei index_count       = 0;
  
  GfxBuffer* instance_buffer = nullptr; 
  sizei instance_count       = 0;

  GfxDrawMode draw_mode;
};
/// GfxPipeline
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Private functions

static void set_gfx_states(GfxContext* gfx) {
  gl_set_depth_state(gfx->desc.depth_desc);
  gl_set_stencil_state(gfx->desc.stencil_desc);
  gl_set_blend_state(gfx->desc.blend_desc);
  gl_set_cull_state(gfx->desc.cull_desc);

  if(IS_BIT_SET(gfx->states, GFX_STATE_DEPTH)) {
    gl_set_state(GFX_STATE_DEPTH, true);   
    gfx->default_clear_flags |= GL_DEPTH_BUFFER_BIT;
  }
  
  if(IS_BIT_SET(gfx->states, GFX_STATE_STENCIL)) {
    gl_set_state(GFX_STATE_STENCIL, true);   
    gfx->default_clear_flags |= GL_STENCIL_BUFFER_BIT;
  }
  
  if(IS_BIT_SET(gfx->states, GFX_STATE_BLEND)) {
    gl_set_state(GFX_STATE_BLEND, true);   
  }
  
  if(IS_BIT_SET(gfx->states, GFX_STATE_MSAA)) {
    gl_set_state(GFX_STATE_MSAA, true);   
  }

  if(IS_BIT_SET(gfx->states, GFX_STATE_CULL)) {
    gl_set_state(GFX_STATE_CULL, true);   
  }
  
  if(IS_BIT_SET(gfx->states, GFX_STATE_SCISSOR)) {
    gl_set_state(GFX_STATE_SCISSOR, true);   
  }
}

static void init_pipeline_layout(GfxPipeline* pipe, sizei* strides) {
  // Set the layouts of the buffers

  for(sizei i = 0; i < VERTEX_LAYOUTS_MAX; i++) {
    FREYA_DEBUG_ASSERT((pipe->desc.layouts[i].attributes_count >= 0) && (pipe->desc.layouts[i].attributes_count < VERTEX_ATTRIBUTES_MAX), 
                       "Attributes count cannot exceed VERTEX_ATTRIBUTES_MAX");

    // Set the attributes of the buffer

    sizei start  = pipe->desc.layouts[i].start_index;
    sizei stride = 0; 

    for(sizei j = 0; j < pipe->desc.layouts[i].attributes_count; j++) {
      GfxLayoutType attribute = pipe->desc.layouts[i].attributes[j];
      
      GLenum gl_comp_type = gl_get_layout_type(attribute);
      sizei comp_count    = gl_get_layout_count(attribute);
      sizei size          = gl_get_layout_size(attribute);

      /// @NOTE:
      /// 
      /// This is just a temporary resolution until I can fix this much later. 
      /// If I stop being lazy, that is, which is unlikely to happen for a while. 
      ///
      /// Essentially, we are assuming that all floating-point attributes will _NOT_ 
      /// be normalized. That's because we usually don't want them to be. However, 
      /// with other non floating-point numbers, we assume that we need them to be normalized.
      /// With bytes, for example, we might need to normalize that since we might be using them 
      /// as color values. 
      ///
      /// This might be problematic, so I might come back here and fix it... 
      ///

      bool is_normalized = (gl_comp_type != GL_FLOAT);

      glEnableVertexArrayAttrib(start + j);
      glVertexArrayAttribFormat(start + j, comp_count, gl_comp_type, is_normalized, stride);
      glVertexArrayAttribBinding(start + j, i);
      
      // Increase the stride for the next round
      stride += size;
    }

    strides[i] = stride;
    glVertexArrayBindingDivisor(pipe->vertex_array, i, pipe->desc.layouts[i].instance_rate);
  }
}

/// Private functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Context functions 

GfxContext* gfx_context_init(const GfxContextDesc& desc) {
  GfxContext* gfx = (GfxContext*)memory_allocate(sizeof(GfxContext));
  memory_zero(gfx, sizeof(GfxContext)); 
  
  gfx->desc                = desc;
  gfx->default_clear_flags = GL_COLOR_BUFFER_BIT;
  gfx->current_clear_flags = gfx->default_clear_flags;

  // Glad init
  
  if(!gladLoadGLES2Loader(glfwGetProcAddress)) {
    FREYA_LOG_FATAL("Could not create an OpenGL-ES instance");
    return nullptr;
  }

  // Setting the window context to this OpenGL context 
  window_set_current_context(desc.window);

  // Setting up the viewport for OpenGL
  
  IVec2 window_size = window_get_framebuffer_size(desc.window);
  glViewport(0, 0, window_size.x, window_size.y);

  // Setting the flags
  
  gfx->states = (GfxStates)desc.states;
  set_gfx_states(gfx);

  // Getting the version number
  
  i32 major_ver, minor_ver;
  glGetIntegerv(GL_MAJOR_VERSION, &major_ver);
  glGetIntegerv(GL_MINOR_VERSION, &minor_ver);
  gl_check_supported_version(major_ver, minor_ver);

  // Getting some OpenGL information
  
  const u8* vendor       = glGetString(GL_VENDOR); 
  const u8* renderer     = glGetString(GL_RENDERER); 
  const u8* gl_version   = glGetString(GL_VERSION);
  const u8* glsl_version = glGetString(GL_SHADING_LANGUAGE_VERSION);

  // Get the current extensions

  i32 extensions_count = 0; 
  glGetIntegerv(GL_NUM_EXTENSIONS, &extensions_count);

  gfx->extensions.resize(extensions_count);

  for(i32 i = 0; i < extensions_count; i++) {
    String ext = (const char*)glGetStringi(GL_EXTENSIONS, i);
    gfx->extensions[i] = ext;
  }

  // Listen to some events
  event_register(EVENT_WINDOW_FRAMEBUFFER_RESIZED, gl_on_framebuffer_resize_callback, gfx);

  // Some useful info dump

  FREYA_LOG_INFO("An OpenGL-ES graphics context was successfully created:\n" 
                 "              Vendor: %s\n" 
                 "              Renderer: %s\n" 
                 "              GLES version: %s\n" 
                 "              GLSL version: %s\n" 
                 "              Extensions count: %zu", 
                 vendor, renderer, gl_version, glsl_version, gfx->extensions.size());

  return gfx;
}

void gfx_context_shutdown(GfxContext* gfx) {
  if(!gfx) {
    return;
  }

  FREYA_LOG_INFO("The graphics context was successfully destroyed");
  memory_free(gfx);
}

GfxContextDesc& gfx_context_get_desc(GfxContext* gfx) {
  FREYA_DEBUG_ASSERT(gfx, "Invalid GfxContext struct passed");
  return gfx->desc;
}

bool gfx_context_has_extension(GfxContext* gfx, const char* ext) {
  FREYA_DEBUG_ASSERT(gfx, "Invalid GfxContext struct passed");

  for(auto& ext_str : gfx->extensions) {
    if(ext_str == ext) {
      return true;
    }
  }

  return false;
}

void gfx_context_set_state(GfxContext* gfx, const GfxStates state, const bool value) {
  FREYA_DEBUG_ASSERT(gfx, "Invalid GfxContext struct passed");
  gl_set_state(state, value);
}

void gfx_context_set_depth_state(GfxContext* gfx, const GfxDepthDesc& depth_desc) {
  FREYA_DEBUG_ASSERT(gfx, "Invalid GfxContext struct passed");
  
  gfx->desc.depth_desc = depth_desc; 
  gl_set_depth_state(depth_desc);
}

void gfx_context_set_stencil_state(GfxContext* gfx, const GfxStencilDesc& stencil_desc) {
  FREYA_DEBUG_ASSERT(gfx, "Invalid GfxContext struct passed");
  
  gfx->desc.stencil_desc = stencil_desc; 
  gl_set_stencil_state(stencil_desc);
}

void gfx_context_set_cull_state(GfxContext* gfx, const GfxCullDesc& cull_desc) {
  FREYA_DEBUG_ASSERT(gfx, "Invalid GfxContext struct passed");
  
  gfx->desc.cull_desc = cull_desc; 
  gl_set_cull_state(cull_desc);
}

void gfx_context_set_blend_state(GfxContext* gfx, const GfxBlendDesc& blend_desc) {
  FREYA_DEBUG_ASSERT(gfx, "Invalid GfxContext struct passed");
  
  gfx->desc.blend_desc = blend_desc; 
  gl_set_blend_state(blend_desc);
} 

void gfx_context_set_scissor_rect(GfxContext* gfx, const i32 x, const i32 y, const i32 width, const i32 height) {
  FREYA_DEBUG_ASSERT(gfx, "Invalid GfxContext struct passed");
  glScissor(x, y, width, height);
}

void gfx_context_set_viewport(GfxContext* gfx, const i32 x, const i32 y, const i32 width, const i32 height) {
  FREYA_DEBUG_ASSERT(gfx, "Invalid GfxContext struct passed");
  glViewport(x, y, width, height);
}

void gfx_context_set_target(GfxContext* gfx, GfxFramebuffer* framebuffer) {
  FREYA_DEBUG_ASSERT(gfx, "Invalid GfxContext struct passed");

  // Set the default values for when binding to the default framebuffer
  
  gfx->current_clear_flags = gfx->default_clear_flags;
  gfx->current_target      = 0;

  if(framebuffer) {
    gfx->current_clear_flags = framebuffer->clear_flags;
    gfx->current_target      = framebuffer->id; 
  }
}

void gfx_context_clear(GfxContext* gfx, const f32 r, const f32 g, const f32 b, const f32 a) {
  FREYA_DEBUG_ASSERT(gfx, "Invalid GfxContext struct passed");

  glBindFramebuffer(GL_FRAMEBUFFER, gfx->current_target);
  glClear(gfx->current_clear_flags);
  glClearColor(r, g, b, a);
}

void gfx_context_use_bindings(GfxContext* gfx, const GfxBindingDesc& binding_desc) {
  FREYA_DEBUG_ASSERT(gfx, "Invalid GfxContext struct passed");
  FREYA_DEBUG_ASSERT(binding_desc.shader, "Must have a valid GfxShader to bind resources");

  // Bind the shader
  glUseProgram(binding_desc.shader->id);

  // Bind the textures 

  FREYA_DEBUG_ASSERT(((binding_desc.textures_count >= 0) && (binding_desc.textures_count < TEXTURES_MAX)), 
      "Textures count in gfx_context_use_bindings exceeding TEXTURES_MAX");

  for(sizei i = 0; i < binding_desc.textures_count; i++) {
    GfxTexture* texture = binding_desc.textures[i];

    FREYA_DEBUG_ASSERT(texture, "An invalid texture found in texutres array");
    glBindTexture(texture->gl_type, texture->id);
    glActivateTexture(GL_TEXTURE0 + i);
  }

  // Bind the images
  
  FREYA_DEBUG_ASSERT(((binding_desc.images_count >= 0) && (binding_desc.images_count < TEXTURES_MAX)), 
                "Images count in gfx_context_use_bindings exceeding TEXTURES_MAX");
  
  for(sizei i = 0; i < binding_desc.images_count; i++) {
    GfxTexture* image = binding_desc.images[i];

    FREYA_DEBUG_ASSERT(image, "An invalid texture found in images array");
  
    GLenum access = gl_get_texture_access(image->desc.access);
    GLenum in_format, gl_format, gl_pixel_type;
    gl_get_texture_format(image->desc.format, &in_format, &gl_format, &gl_pixel_type);

    glBindImageTexture(i,          // Image unit
                       image->id,  // Image ID 
                       0,          // Level
                       false,      // Layered state (for texture arrays)
                       0,          // Layer index
                       access,     // Image access type
                       in_format); // Image format            
  }

  // Bind the buffers

  for(sizei i = 0; i < binding_desc.buffers_count; i++) {
    GfxBuffer* buffer = binding_desc.buffers[i];
    
    FREYA_DEBUG_ASSERT(binding_desc.buffers[i], "An invalid buffer found in buffers array");
    glBindBuffer(buffer->gl_buff_type, buffer->id);
  }

  // Bind the cubemaps
  
  FREYA_DEBUG_ASSERT(((binding_desc.cubemaps_count >= 0) && (binding_desc.cubemaps_count < CUBEMAPS_MAX)), 
                "Cubemaps count in gfx_context_use_bindings exceeding CUBEMAPS_MAX");

  for(sizei i = 0; i < binding_desc.cubemaps_count; i++) {
    GfxCubemap* cubemap = binding_desc.cubemaps[i];

    FREYA_DEBUG_ASSERT(cubemap, "An invalid cubemap found in cubemaps array");
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap->id);
    glActivateTexture(GL_TEXTURE0 + i);
  }
}

void gfx_context_use_pipeline(GfxContext* gfx, GfxPipeline* pipeline) {
  FREYA_DEBUG_ASSERT(gfx, "Invalid GfxContext struct passed");
  FREYA_DEBUG_ASSERT(pipeline, "Invalid GfxPipeline struct passed");
  FREYA_DEBUG_ASSERT(pipeline->vertex_buffer, "Must at least have a valid vertex buffer to draw");

  // Use the pipline data to setup for the next draw call

  pipeline->gfx->bound_pipeline = pipeline;

  glDepthMask(pipeline->desc.depth_mask);
  glStencilMask(pipeline->desc.stencil_ref);

  const f32* blend_color = pipeline->desc.blend_factor;
  glBlendColor(blend_color[0], blend_color[1], blend_color[2], blend_color[3]);

  // Bind the new bound pipeline
  glBindVertexArray(pipeline->vertex_array);
}

void gfx_context_draw(GfxContext* gfx, const u32 start_element) {
  FREYA_DEBUG_ASSERT(gfx, "Invalid GfxContext struct passed");
  FREYA_DEBUG_ASSERT(gfx->bound_pipeline, "Cannot draw using an invalid bound pipeline");

  GfxPipeline* pipe = gfx->bound_pipeline;
  GLenum draw_mode  = gl_get_draw_mode(pipe->desc.draw_mode);

  // Draw the index buffer (if it is valid).
  // Otherwise, draw using the vertex buffer.
  
  if(pipe->index_buffer) {
    GLenum index_type = gl_get_layout_type(pipe->desc.indices_type);
    glDrawElements(draw_mode, pipe->index_count, index_type, 0);
  }
  else {
    glDrawArrays(draw_mode, start_element, pipe->vertex_count);
  }

  // Unbind the vertex array for debugging purposes
  glBindVertexArray(0);
}

void gfx_context_draw_instanced(GfxContext* gfx, const u32 start_element) {
  FREYA_DEBUG_ASSERT(gfx, "Invalid GfxContext struct passed");
  FREYA_DEBUG_ASSERT(gfx->bound_pipeline, "Cannot draw using an invalid bound pipeline");
  FREYA_DEBUG_ASSERT(gfx->bound_pipeline->instance_buffer, "Cannot instance draw using an invalid instance buffer");

  GfxPipeline* pipe = gfx->bound_pipeline;
  GLenum draw_mode  = gl_get_draw_mode(pipe->desc.draw_mode);
  
  // Draw the index buffer (if it is valid).
  // Otherwise, draw using the vertex buffer.
  
  if(pipe->index_buffer) {
    GLenum index_type = gl_get_layout_type(pipe->desc.indices_type);
    glDrawElementsInstanced(draw_mode, pipe->index_count, index_type, 0, pipe->instance_count);
  }
  else {
    glDrawArraysInstanced(draw_mode, start_element, pipe->vertex_count, pipe->instance_count);
  }
  
  // Unbind the vertex array for debugging purposes
  glBindVertexArray(0);
}

void gfx_context_draw_multi_indirect(GfxContext* gfx, const u32 offset, const sizei count, const sizei stride) {
  FREYA_DEBUG_ASSERT(gfx, "Invalid GfxContext struct passed");
  
  GfxPipeline* pipe = gfx->bound_pipeline;
  GLenum draw_mode  = gl_get_draw_mode(pipe->desc.draw_mode);
  
  // Draw the index buffer (if it is valid).
  // Otherwise, draw using the vertex buffer.
  
  if(pipe->index_buffer) {
    GLenum index_type = gl_get_layout_type(pipe->desc.indices_type);
    glMultiDrawElementsIndirect(draw_mode, index_type, nullptr, count, stride);
  }
  else {
    glMultiDrawArraysIndirect(draw_mode, nullptr, count, stride);
  }
  
  // Unbind the vertex array for debugging purposes
  glBindVertexArray(0);
}

void gfx_context_dispatch(GfxContext* gfx, const u32 work_group_x, const u32 work_group_y, const u32 work_group_z) {
  FREYA_DEBUG_ASSERT(gfx, "Invalid GfxContext struct passed");

  // Some bookkeeping...

  bool is_group_x_count_valid = (work_group_x >= 1) && (work_group_x < MAX_COMPUTE_WORK_GROUPS_COUNT);
  bool is_group_y_count_valid = (work_group_y >= 1) && (work_group_y < MAX_COMPUTE_WORK_GROUPS_COUNT);
  bool is_group_z_count_valid = (work_group_z >= 1) && (work_group_z < MAX_COMPUTE_WORK_GROUPS_COUNT);

  FREYA_DEBUG_ASSERT((is_group_x_count_valid && is_group_y_count_valid && is_group_z_count_valid), 
                "Invalid work group counts given to gfx_context_dispatch");

  // Some computing incoming!
  glDispatchCompute(work_group_x, work_group_y, work_group_z);
}

void gfx_context_memory_barrier(GfxContext* gfx, const i32 barrier_bits) {
  FREYA_DEBUG_ASSERT(gfx, "Invalid GfxContext struct passed");
  
  GLbitfield barriers = 0;
 
  // Unset and check the buffer bits and set them to OpenGL equivalents
 
  // 10 = the maximum number of barriers
  // @FIX (GL-Backend): A magic number like that is probably not the best idea...
  
  for(sizei i = 0; i < 10; i++) {
    i32 type = (GFX_MEMORY_BARRIER_VERTEX_ATTRIBUTE << i);

    if(IS_BIT_SET(barriers, type)) {
      barriers |= gl_get_barrier((GfxMemoryBarrierType)type);
    }
  }

  glMemoryBarrier(barriers);
}

void gfx_context_present(GfxContext* gfx) {
  FREYA_DEBUG_ASSERT(gfx, "Invalid GfxContext struct passed");
  window_swap_buffers(gfx->desc.window, gfx->desc.has_vsync);
}

/// Context functions 
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Framebuffer functions

GfxFramebuffer* gfx_framebuffer_create(GfxContext* gfx, const GfxFramebufferDesc& desc, const AllocateMemoryFn& alloc_fn) {
  FREYA_DEBUG_ASSERT(gfx, "Invalid GfxContext struct passed");

  bool is_count_valid = (desc.attachments_count >= 0) && (desc.attachments_count < FRAMEBUFFER_ATTACHMENTS_MAX);
  FREYA_DEBUG_ASSERT(is_count_valid, "Attachments count in GfxFramebuffer cannot exceed FRAMEBUFFER_ATTACHMENTS_MAX");

  GfxFramebuffer* buff = (GfxFramebuffer*)alloc_fn(sizeof(GfxFramebuffer));

  buff->desc        = desc; 
  buff->clear_flags = gl_get_clear_flags(desc.clear_flags);

  glGenFramebuffers(1, &buff->id);
  glBindFramebuffer(GL_FRAMEBUFFER, buff->id);

  // Attach color attachments

  for(sizei i = 0; i < desc.attachments_count; i++) {
    glFramebufferTexture2D(GL_FRAMEBUFFER, 
                           GL_COLOR_ATTACHMENT0 + i, 
                           desc.color_attachments[i]->gl_type, 
                           desc.color_attachments[i]->id, 
                           0);
    buff->color_textures[i] = GL_COLOR_ATTACHMENT0 + i;
  }

  // Attach the depth attachments (if it exists)
  
  if(desc.depth_attachment) {
    // The depth attachment can either be a regular texture 
    // tha can be sampled from or a render buffer object for 
    // write-only purposes.
     
    GLenum depth_type = gl_get_attachment_type(desc.depth_attachment->desc.format); 

    if(glIsRenderbuffer(desc.depth_attachment->id)) {
      GLenum access_type = (desc.depth_attachment->desc.access == GFX_TEXTURE_ACCESS_WRITE) ? GL_FRAMEBUFFER_DRAW : GL_FRAMEBUFFER_READ;
      glFramebufferRenderbuffer(access_type, 
                                depth_type, 
                                GL_RENDERBUFFER, 
                                desc.depth_attachment->id);
    }
    else {
      glFramebufferTexture2D(GL_FRAMEBUFFER, 
                             depth_type, 
                             desc.depth_attachment->gl_type, 
                             desc.depth_attachment->id, 
                             0);
    }
    
    buff->depth_texture = depth_type;
  }
  
  // Attach the stencil attachments (if it exists)
  
  if(desc.stencil_attachment) {
    // The stencil attachment can either be a regular texture 
    // tha can be sampled from or a render buffer object for 
    // write-only purposes.

    if(glIsRenderbuffer(desc.stencil_attachment->id)) {
      GLenum access_type = (desc.stencil_attachment->desc.access == GFX_TEXTURE_ACCESS_WRITE) ? GL_FRAMEBUFFER_DRAW : GL_FRAMEBUFFER_READ;
      glFramebufferRenderbuffer(access_type, 
                                GL_STENCIL_ATTACHMENT, 
                                GL_RENDERBUFFER, 
                                desc.stencil_attachment->id);
    }
    else {
      glFramebufferTexture2D(GL_FRAMEBUFFER, 
                             depth_type, 
                             GL_STENCIL_ATTACHMENT, 
                             desc.stencil_attachment->id, 
                             0);
    }

    buff->stencil_texture = GL_STENCIL_ATTACHMENT;
  }
  
  // Setting the draw and read buffers
  glDrawBuffers(buff->desc.attachments_count, buff->color_textures);
 
  // @TEMP (GL-Backend)
  glReadBuffer(buff->color_textures[0]);

  // Sanity check

  if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    FREYA_LOG_WARN("GL-ERROR: Framebuffer %i is incomplete", buff->id);
  }

  // Done!
  
  glBindFramebuffer(GL_FRAMEBUFFER, gfx->current_target);
  return buff;
}

void gfx_framebuffer_destroy(GfxFramebuffer* framebuffer, const FreeMemoryFn& free_fn) {
  if(!framebuffer) {
    return;
  }

  glDeleteFramebuffers(1, &framebuffer->id);
  free_fn(framebuffer);
}

void gfx_framebuffer_copy(const GfxFramebuffer* src_frame, 
                          GfxFramebuffer* dest_frame, 
                          i32 src_x, i32 src_y, 
                          i32 src_width, i32 src_height, 
                          i32 dest_x, i32 dest_y, 
                          i32 dest_width, i32 dest_height, 
                          i32 buffer_mask) {
  FREYA_DEBUG_ASSERT((src_frame || dest_frame), "Cannot have both framebuffers as NULL in copy operation");

  // Getting the correct values

  u32 src_id   = src_frame ? src_frame->id : 0;
  u32 dest_id  = dest_frame ? dest_frame->id : 0;
  u32 gl_masks = gl_get_clear_flags(buffer_mask);

  // Blitting the buffers

  glBindFramebuffer(GL_READ_FRAMEBUFFER, src_frame->id);
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, dest_frame->id);

  glBlitFramebuffer(src_x, src_y, 
                    src_width, src_height, 
                    dest_x, dest_y, 
                    dest_width, dest_height, 
                    gl_masks, 
                    GL_NEAREST);
}

GfxFramebufferDesc& gfx_framebuffer_get_desc(GfxFramebuffer* framebuffer) {
  FREYA_DEBUG_ASSERT(framebuffer, "Invalid GfxFramebuffer struct passed");
  return framebuffer->desc;
}

void gfx_framebuffer_update(GfxFramebuffer* framebuffer, const GfxFramebufferDesc& desc) {
  FREYA_DEBUG_ASSERT(framebuffer, "Invalid GfxFramebuffer struct passed");

  bool is_count_valid = (desc.attachments_count >= 0) && (desc.attachments_count < FRAMEBUFFER_ATTACHMENTS_MAX);
  FREYA_DEBUG_ASSERT(is_count_valid, "Attachments count in GfxFramebuffer cannot exceed FRAMEBUFFER_ATTACHMENTS_MAX");
  
  framebuffer->desc        = desc; 
  framebuffer->clear_flags = gl_get_clear_flags(desc.clear_flags);

  glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->id);

  // Attach color attachments

  for(sizei i = 0; i < desc.attachments_count; i++) {
    glFramebufferTexture2D(GL_FRAMEBUFFER, 
                           GL_COLOR_ATTACHMENT0 + i, 
                           desc.color_attachments[i]->gl_type, 
                           desc.color_attachments[i]->id, 
                           0);
    framebuffer->color_textures[i] = GL_COLOR_ATTACHMENT0 + i;
  }

  // Attach the depth attachments (if it exists)
  
  if(desc.depth_attachment) {
    // The depth attachment can either be a regular texture 
    // tha can be sampled from or a render buffer object for 
    // write-only purposes.
     
    GLenum depth_type = gl_get_attachment_type(desc.depth_attachment->desc.format); 

    if(glIsRenderbuffer(desc.depth_attachment->id)) {
      GLenum access_type = (desc.depth_attachment->desc.access == GFX_TEXTURE_ACCESS_WRITE) ? GL_FRAMEBUFFER_DRAW : GL_FRAMEBUFFER_READ;
      glFramebufferRenderbuffer(access_type, 
                                depth_type, 
                                GL_RENDERBUFFER, 
                                desc.depth_attachment->id);
    }
    else {
      glFramebufferTexture2D(GL_FRAMEBUFFER, 
                             depth_type, 
                             desc.depth_attachment->gl_type, 
                             desc.depth_attachment->id, 
                             0);
    }
    
    framebuffer->depth_texture = depth_type;
  }
  
  // Attach the stencil attachments (if it exists)
  
  if(desc.stencil_attachment) {
    // The stencil attachment can either be a regular texture 
    // tha can be sampled from or a render buffer object for 
    // write-only purposes.

    if(glIsRenderbuffer(desc.stencil_attachment->id)) {
      GLenum access_type = (desc.stencil_attachment->desc.access == GFX_TEXTURE_ACCESS_WRITE) ? GL_FRAMEBUFFER_DRAW : GL_FRAMEBUFFER_READ;
      glFramebufferRenderbuffer(access_type, 
                                GL_STENCIL_ATTACHMENT, 
                                GL_RENDERBUFFER, 
                                desc.stencil_attachment->id);
    }
    else {
      glFramebufferTexture2D(GL_FRAMEBUFFER, 
                             depth_type, 
                             GL_STENCIL_ATTACHMENT, 
                             desc.stencil_attachment->id, 
                             0);
    }

    framebuffer->stencil_texture = GL_STENCIL_ATTACHMENT;
  }
  
  // Setting the draw and read buffers
  glDrawBuffers(framebuffer->desc.attachments_count, framebuffer->color_textures);
 
  // @TEMP (GL-Backend)
  glReadBuffer(framebuffer->color_textures[0]);

  // Sanity check

  if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    FREYA_LOG_WARN("GL-ERROR: Framebuffer %i is incomplete", framebuffer->id);
  }

  // Done!
  glBindFramebuffer(GL_FRAMEBUFFER, gfx->current_target);
}

/// Framebuffer functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Buffer functions 

GfxBuffer* gfx_buffer_create(GfxContext* gfx, const AllocateMemoryFn& alloc_fn) {
  FREYA_DEBUG_ASSERT(gfx, "Invalid GfxContext struct passed");

  GfxBuffer* buff = (GfxBuffer*)alloc_fn(sizeof(GfxBuffer));
  
  buff->desc = {};
  buff->gfx  = gfx; 

  glGenBuffers(1, &buff->id);
  return buff;
}

const bool gfx_buffer_load(GfxBuffer* buffer, const GfxBufferDesc& desc) {
  FREYA_DEBUG_ASSERT(buffer, "Trying to load data into an invalid resource");
 
  buffer->desc          = desc;
  buffer->gl_buff_type  = gl_get_buffer_type(desc.type);
  buffer->gl_buff_usage = gl_get_buffer_usage(desc.usage);

  glBindBuffer(buffer->gl_buff_type, buffer->id);
  glBufferData(buffer->gl_buff_type, desc.size, desc.data, buffer->gl_buff_usage);
  
  return true;
}

void gfx_buffer_destroy(GfxBuffer* buff, const FreeMemoryFn& free_fn) {
  if(!buff) {
    return;
  }

  glDeleteBuffers(1, &buff->id);
  free_fn(buff);
}

GfxBufferDesc& gfx_buffer_get_desc(GfxBuffer* buffer) {
  FREYA_DEBUG_ASSERT(buffer, "Invalid GfxBuffer struct passed");
  return buffer->desc;
}

void gfx_buffer_bind_point(GfxBuffer* buffer, const u32 bind_point) {
  FREYA_DEBUG_ASSERT(buffer, "Invalid GfxBuffer struct passed");
  
  bool is_valid_buffer = (buffer->desc.type == GFX_BUFFER_UNIFORM) || (buffer->desc.type == GFX_BUFFER_SHADER_STORAGE);
  FREYA_DEBUG_ASSERT(is_valid_buffer, "Cannot bind a non-uniform or non-shader storage buffer to a bind point");

  glBindBufferBase(gl_get_buffer_type(buffer->desc.type), bind_point, buffer->id);
}

void gfx_buffer_update(GfxBuffer* buff, const GfxBufferDesc& desc) {
  FREYA_DEBUG_ASSERT(buff, "Invalid GfxBuffer struct passed");
  FREYA_DEBUG_ASSERT(buff->gfx, "Invalid GfxContext struct passed");

  buff->desc = desc;
}

void gfx_buffer_upload_data(GfxBuffer* buff, const sizei offset, const sizei size, const void* data) {
  FREYA_DEBUG_ASSERT(buff, "Invalid GfxBuffer struct passed");
  FREYA_DEBUG_ASSERT(buff->gfx, "Invalid GfxContext struct passed");
  FREYA_DEBUG_ASSERT((offset + size) <= buff->desc.size, "The GfxBuffer does not have enough memory to upload this data");
  
  glBindBuffer(buff->gl_buff_type, buff->id);
  glBufferSubData(buff->gl_buff_type, offset, size, data);
}

/// Buffer functions 
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Shader functions 

GfxShader* gfx_shader_create(GfxContext* gfx, const AllocateMemoryFn& alloc_fn) {
  FREYA_DEBUG_ASSERT(gfx, "Invalid GfxContext struct passed");

  GfxShader* shader = (GfxShader*)alloc_fn(sizeof(GfxShader));

  shader->desc = {};
  shader->gfx  = gfx;
  shader->id   = glCreateProgram();

  return shader;
}

const bool gfx_shader_load(GfxShader* shader, const GfxShaderDesc& desc) {
  FREYA_DEBUG_ASSERT(shader, "Trying to load data into an invalid resource");
  
  shader->desc = desc;

  // Create the compute shader if the source exists. 
  // Do not continue if this is true, since compute 
  // shaders are quite special flakes. 

  if(!desc.compute_source.empty()) {
    shader->compute_id = glCreateShader(GL_COMPUTE_SHADER);
    
    i32 compute_src_len     = (i32)shader->desc.compute_source.size();
    const char* compute_str = shader->desc.compute_source.c_str();

    glShaderSource(shader->compute_id, 1, &compute_str, &compute_src_len); 
    glCompileShader(shader->compute_id);
    
    gl_check_shader_compile_error(shader->compute_id);
    glAttachShader(shader->id, shader->compute_id);

    // Linking

    glLinkProgram(shader->id);
    gl_check_shader_linker_error(shader->id);

    return true;
  }

  // Necessary asserts

  FREYA_DEBUG_ASSERT(!desc.vertex_source.empty(), "Invalid Vertex source passed to the shader");
  FREYA_DEBUG_ASSERT(!desc.pixel_source.empty(), "Invalid Pixel source passed to the shader");

  // Vertex shader
  
  shader->vert_id = glCreateShader(GL_VERTEX_SHADER);
  
  i32 vert_src_len       = (i32)shader->desc.vertex_source.size();
  const char* vertex_str = shader->desc.vertex_source.c_str();
  
  glShaderSource(shader->vert_id, 1, &vertex_str, &vert_src_len); 
  glCompileShader(shader->vert_id);
  
  gl_check_shader_compile_error(shader->vert_id);
  glAttachShader(shader->id, shader->vert_id);
   
  // Fragment shader
  
  shader->frag_id = glCreateShader(GL_FRAGMENT_SHADER);
  
  i32 frag_src_len     = (i32)shader->desc.pixel_source.size();
  const char* frag_str = shader->desc.pixel_source.c_str();
  
  glShaderSource(shader->frag_id, 1, &frag_str, &frag_src_len); 
  glCompileShader(shader->frag_id);

  gl_check_shader_compile_error(shader->frag_id);
  glAttachShader(shader->id, shader->frag_id);

  // Linking
  
  glLinkProgram(shader->id);
  gl_check_shader_linker_error(shader->id);
 
  // Done!
  return true;
}

void gfx_shader_destroy(GfxShader* shader, const FreeMemoryFn& free_fn) {
  if(!shader) {
    return;
  }

  glDeleteProgram(shader->id);
  free_fn(shader);
}

GfxShaderDesc& gfx_shader_get_source(GfxShader* shader) {
  FREYA_DEBUG_ASSERT(shader, "Invalid GfxShader struct passed to gfx_shader_get_source");
  return shader->desc;
}

void gfx_shader_update(GfxShader* shader, const GfxShaderDesc& desc) {
  FREYA_DEBUG_ASSERT(shader->gfx, "Invalid GfxContext struct passed");
  FREYA_DEBUG_ASSERT(shader, "Invalid GfxShader struct passed");
  
  shader->desc = desc;

  // Create the compute shader if the source exists. 
  // Do not continue if this is true, since compute 
  // shaders are quite special flakes. 

  if(!desc.compute_source.empty()) {
    i32 compute_src_len     = (i32)shader->desc.compute_source.size();
    const char* compute_str = shader->desc.compute_source.c_str();

    glShaderSource(shader->compute_id, 1, &compute_str, &compute_src_len); 
    glCompileShader(shader->compute_id);
    
    gl_check_shader_compile_error(shader->compute_id);
    glAttachShader(shader->id, shader->compute_id);

    // Linking

    glLinkProgram(shader->id);
    gl_check_shader_linker_error(shader->id);

    return;
  }

  // Necessary asserts

  FREYA_DEBUG_ASSERT(!desc.vertex_source.empty(), "Invalid Vertex source passed to the shader");
  FREYA_DEBUG_ASSERT(!desc.pixel_source.empty(), "Invalid Pixel source passed to the shader");

  // Vertex shader
  
  i32 vert_src_len       = (i32)shader->desc.vertex_source.size();
  const char* vertex_str = shader->desc.vertex_source.c_str();
  
  glShaderSource(shader->vert_id, 1, &vertex_str, &vert_src_len); 
  glCompileShader(shader->vert_id);
  
  gl_check_shader_compile_error(shader->vert_id);
  glAttachShader(shader->id, shader->vert_id);
   
  // Fragment shader
  
  i32 frag_src_len     = (i32)shader->desc.pixel_source.size();
  const char* frag_str = shader->desc.pixel_source.c_str();
  
  glShaderSource(shader->frag_id, 1, &frag_str, &frag_src_len); 
  glCompileShader(shader->frag_id);
  
  gl_check_shader_compile_error(shader->frag_id);
  glAttachShader(shader->id, shader->frag_id);

  // Linking
  
  glLinkProgram(shader->id);
  gl_check_shader_linker_error(shader->id);
}

void gfx_shader_query(GfxShader* shader, GfxShaderQueryDesc* out_desc) {
  FREYA_DEBUG_ASSERT(shader->gfx, "Invalid GfxContext struct passed");
  FREYA_DEBUG_ASSERT(shader, "Invalid GfxShader struct passed");
 
  // Check if the shader is actually linked properly
 
  i32 success; 
  char log_info[MAX_SHADER_LOG_MSG_LENGTH];

  glGetProgramiv(shader->id, GL_LINK_STATUS, &success); 
  if(!success) {
    glGetProgramInfoLog(shader->id, 1024, nullptr, log_info);
    FREYA_LOG_WARN("SHADER-ERROR: %s", log_info);

    return;
  }
  
  i32 max_name_len = 0;

  // Retrieve the attributes information

  glGetProgramiv(shader->id, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &max_name_len); 
  glGetProgramiv(shader->id, GL_ACTIVE_ATTRIBUTES, &out_desc->attributes_count);
  
  for(i32 i = 0; i < out_desc->attributes_count; i++) {
    GLenum gl_type = (GLenum)0;
    i32 comp_count = 0; 
    char attr_name[512];

    glGetActiveAttrib(shader->id,   // Program
                      i,            // Attribute index
                      max_name_len, // Max name length
                      nullptr,      // Out name length
                      &comp_count,  // Attribute component count
                      &gl_type,     // Attribute type
                      attr_name);   // Attribute name
  
    out_desc->active_attributes[i] = gl_get_uniform_type(gl_type);
  } 
  
  // Retrieve the uniform blocks information
  
  glGetProgramiv(shader->id, GL_ACTIVE_UNIFORM_BLOCK_MAX_NAME_LENGTH, &max_name_len); 
  glGetProgramiv(shader->id, GL_ACTIVE_UNIFORM_BLOCKS, &out_desc->uniform_blocks_count);

  i32 uniform_blocks_active_uniforms = 0;

  for(i32 i = 0; i < out_desc->uniform_blocks_count; i++) {
    glGetActiveUniformBlockiv(shader->id,                           // Program
                              i,                                    // Block index
                              GL_UNIFORM_BLOCK_BINDING,             // Query block binding
                              &out_desc->active_uniform_blocks[i]); // Block param
    
    glGetActiveUniformBlockiv(shader->id,                       // Program
                              i,                                // Block index
                              GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS, // Query block active uniforms
                              &uniform_blocks_active_uniforms); // Block param
  }
  
  // Retrieve the uniforms information
  
  glGetProgramiv(shader->id, GL_ACTIVE_UNIFORM_MAX_LENGTH, &max_name_len); 
  glGetProgramiv(shader->id, GL_ACTIVE_UNIFORMS, &out_desc->uniforms_count);

  for(i32 i = 0; i < out_desc->uniforms_count; i++) {
    GLsizei name_len = 0; 
    i32 comp_count   = 0;
    GLenum gl_type   = (GLenum)0;

    GfxUniformDesc uniform_desc = {};

    glGetActiveUniform(shader->id,        // Program
                      i,                  // Uniform index
                      max_name_len,       // Max name length
                      &name_len,          // Out name length
                      &comp_count,        // Uniform component count
                      &gl_type,           // Uniform type
                      uniform_desc.name); // Uniform name

    uniform_desc.type            = gl_get_uniform_type(gl_type);
    uniform_desc.location        = gfx_shader_uniform_lookup(shader, uniform_desc.name);
    uniform_desc.component_count = comp_count;
    out_desc->active_uniforms[i] = uniform_desc;
  }

  // Retrieve the compute shader work groups information (if a compute shader exists)
 
  i32 shaders_count = 0; 
  glGetProgramiv(shader->id, GL_ATTACHED_SHADERS, &shaders_count);
  if(shader->compute_id <= 0) { // The compute shader ID has not been generated
    return;
  }

  i32 groups[3]; 
  glGetProgramiv(shader->id, GL_COMPUTE_WORK_GROUP_SIZE, groups);

  out_desc->work_group_x = groups[0];
  out_desc->work_group_y = groups[1];
  out_desc->work_group_z = groups[2];
}

i32 gfx_shader_uniform_lookup(GfxShader* shader, const i8* uniform_name) {
  FREYA_DEBUG_ASSERT(shader, "Invalid GfxShader struct passed");
  return glGetUniformLocation(shader->id, uniform_name);
}

void gfx_shader_upload_uniform_array(GfxShader* shader, const i32 location, const GfxLayoutType type, const void* data, const sizei count) {
  FREYA_DEBUG_ASSERT(shader, "Invalid GfxShader struct passed");

  // Will not do anything with an invalid uniform
  
  if(location == -1) {
    FREYA_LOG_WARN("Cannot set uniform with location -1");
    return;
  }

  glUseProgram(shader->id);

  switch(type) {
    case GFX_LAYOUT_FLOAT1:
      glUniform1fv(location, count, (f32*)data);
      break;
    case GFX_LAYOUT_FLOAT2:
      glUniform2fv(location, count, (f32*)data);
      break;
    case GFX_LAYOUT_FLOAT3:
      glUniform3fv(location, count, (f32*)data);
      break;
    case GFX_LAYOUT_FLOAT4:
      glUniform4fv(location, count, (f32*)data);
      break;
    case GFX_LAYOUT_INT1:
      glUniform1iv(location, count, (i32*)data);
      break;
    case GFX_LAYOUT_INT2:
      glUniform2iv(location, count, (i32*)data);
      break;
    case GFX_LAYOUT_INT3:
      glUniform3iv(location, count, (i32*)data);
      break;
    case GFX_LAYOUT_INT4:
      glUniform4iv(location, count, (i32*)data);
      break;
    case GFX_LAYOUT_UINT1:
      glUniform1uiv(location, count, (u32*)data);
      break;
    case GFX_LAYOUT_UINT2:
      glUniform2uiv(location, count, (u32*)data);
      break;
    case GFX_LAYOUT_UINT3:
      glUniform3uiv(location, count, (u32*)data);
      break;
    case GFX_LAYOUT_UINT4:
      glUniform4uiv(location, count, (u32*)data);
      break;
    case GFX_LAYOUT_MAT2:
      glUniformMatrix2fv(location, count, GL_FALSE, (f32*)data);
      break;
    case GFX_LAYOUT_MAT3:
      glUniformMatrix3fv(location, count, GL_FALSE, (f32*)data);
      break;
    case GFX_LAYOUT_MAT4:
      glUniformMatrix4fv(location, count, GL_FALSE, (f32*)data);
      break;
    default:
      FREYA_LOG_WARN("Shader layout type not supported %i", (u32)type);
      break;
  }
}

void gfx_shader_upload_uniform(GfxShader* shader, const i32 location, const GfxLayoutType type, const void* data) {
  gfx_shader_upload_uniform_array(shader, location, type, data, 1);
}

/// Shader functions 
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Texture functions 

GfxTexture* gfx_texture_create(GfxContext* gfx, const GfxTextureType tex_type, const AllocateMemoryFn& alloc_fn) {
  FREYA_DEBUG_ASSERT(gfx, "Invalid GfxContext struct passed");

  GfxTexture* texture = (GfxTexture*)alloc_fn(sizeof(GfxTexture));

  texture->desc.type   = tex_type; 
  texture->desc        = {};
  texture->gfx         = gfx;
  texture->id          = 0;
  texture->bindless_id = 0;
  texture->gl_type     = gl_get_texture_type(tex_type);
  
  // Creating the texutre based on its type
  
  switch(tex_type) {
    case GFX_TEXTURE_DEPTH_TARGET:
    case GFX_TEXTURE_STENCIL_TARGET:
    case GFX_TEXTURE_DEPTH_STENCIL_TARGET:
      glGenRenderbuffers(1, &texture->id);
      break;
    default:
      glGenTextures(1, &texture->id);
      return nullptr;
  } 

  return texture;
}

const bool gfx_texture_load(GfxTexture* texture, const GfxTextureDesc& desc) {
  FREYA_DEBUG_ASSERT(texture, "Trying to load data into an invalid resource");

  texture->desc = desc;

  // Convert the GFX types into valid GL ones.

  GLenum in_format, gl_format, gl_pixel_type;
  gl_get_texture_format(desc.format, &in_format, &gl_format, &gl_pixel_type);

  GLenum gl_wrap_format = gl_get_texture_wrap(desc.wrap_mode);
  
  GLenum min_filter, mag_filter;
  gl_get_texture_filter(desc.filter, &min_filter, &mag_filter); 

  glBindTexture(texture->gl_type, texture->id);

  // Setting texture parameters (for just images and textures, and not render targets)
 
  if(!glIsRenderbuffer(texture->id)) {
    glTexParameteri(texture->gl_type, GL_TEXTURE_MIN_FILTER, min_filter);
    glTexParameteri(texture->gl_type, GL_TEXTURE_MAG_FILTER, mag_filter);
    glTexParameteri(texture->gl_type, GL_TEXTURE_WRAP_S, gl_wrap_format);
    glTexParameteri(texture->gl_type, GL_TEXTURE_WRAP_T, gl_wrap_format);

    GLint compare_func = (gl_format == GL_DEPTH_COMPONENT) ? GL_COMPARE_REF_TO_TEXTURE : GL_NONE;

    glTexParameteri(texture->gl_type, GL_TEXTURE_COMPARE_MODE, compare_func);
    glTexParameteri(texture->gl_type, GL_TEXTURE_COMPARE_FUNC, gl_get_compare_func(desc.compare_func));
    glTexParameterfv(texture->gl_type, GL_TEXTURE_BORDER_COLOR, desc.border_color);
  }

  // Filling the texture with the data based on its type
 
  gl_set_texture_pixel_align(desc.format);
  gl_update_texture_storage(texture->id, desc, in_format);
  gl_update_texture_pixels(texture->id, desc, gl_format, gl_pixel_type);

  // Generating some mipmaps
  glGenerateMipmap(texture->gl_type);

  // Create the bindless texture handle, and make it 
  // resident in GPU memory, ready to be used. 
  //
  // @TODO (GFX): Perhaps having the texture resident _as soon_ as it 
  // is created is not the best idea.
  //
  
  if(desc.is_bindless) {
    texture->bindless_id = glGetTextureHandleARB(texture->id);
    glMakeTextureHandleResidentARB(texture->bindless_id);
  }

  // Done!
  return true;
}

void gfx_texture_destroy(GfxTexture* texture, const FreeMemoryFn& free_fn) {
  if(!texture) {
    return;
  }
 
  if(texture->desc.is_bindless) {
    glMakeTextureHandleNonResidentARB(texture->bindless_id);
  }
  
  glDeleteTextures(1, &texture->id);
  free_fn(texture);
}

GfxTextureDesc& gfx_texture_get_desc(GfxTexture* texture) {
  FREYA_DEBUG_ASSERT(texture, "Invalid GfxTexture struct passed");

  return texture->desc;
}

const u64 gfx_texture_get_bindless_id(GfxTexture* texture) {
  FREYA_DEBUG_ASSERT(texture, "Invalid GfxTexture struct passed");

  if(!texture->desc.is_bindless) {
    FREYA_LOG_ERROR("Trying to access the bindless ID of a regular texture");
    return 0;
  }

  return texture->bindless_id;
}

const bool gfx_texture_reload(GfxTexture* texture, const GfxTextureDesc& desc) {
  FREYA_DEBUG_ASSERT(texture->gfx, "Invalid GfxContext struct passed to gfx_texture_upload_data");
  FREYA_DEBUG_ASSERT(texture, "Invalid GfxTexture struct passed to gfx_texture_upload_data");

  // Get rid of the texture in the GPU 

  if(desc.is_bindless) {
    glMakeTextureHandleNonResidentARB(texture->bindless_id);
  }
  
  // Creating the new texutre ID based on its type
  
  switch(desc.type) {
    case GFX_TEXTURE_DEPTH_TARGET:
    case GFX_TEXTURE_STENCIL_TARGET:
    case GFX_TEXTURE_DEPTH_STENCIL_TARGET:
      glDeleteRenderbuffers(1, &texture->id);
      glGenRenderbuffers(1, &texture->id);
      break;
    default:
      glDeleteTextures(1, &texture->id);
      glGenTextures(1, &texture->id);
      return false;
  } 

  // Load the new texture
  return gfx_texture_load(texture, desc);
}

void gfx_texture_upload_data(GfxTexture* texture, 
                             const i32 width, const i32 height, const i32 depth, 
                             const void* data) {
  FREYA_DEBUG_ASSERT(texture->gfx, "Invalid GfxContext struct passed to gfx_texture_upload_data");
  FREYA_DEBUG_ASSERT(texture, "Invalid GfxTexture struct passed to gfx_texture_upload_data");
 
  // Updating the formats
  
  GLenum in_format, gl_format, gl_pixel_type;
  gl_get_texture_format(texture->desc.format, &in_format, &gl_format, &gl_pixel_type);
  
  // Update the data
  
  texture->desc.width  = width; 
  texture->desc.height = height; 
  texture->desc.depth  = depth; 
  texture->desc.data   = (void*)data; 

  glBindTexture(texture->gl_type, texture->id);

  // Updating the internal texture pixels
  gl_update_texture_pixels(texture->id, texture->desc, gl_format, gl_pixel_type);

  // Re-generate some mipmaps
  glGenerateMipmap(texture->gl_type);
}

/// Texture functions 
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Cubemap functions 

GfxCubemap* gfx_cubemap_create(GfxContext* gfx, const AllocateMemoryFn& alloc_fn) {
  FREYA_DEBUG_ASSERT(gfx, "Invalid GfxContext struct passed");

  GfxCubemap* cubemap = (GfxCubemap*)alloc_fn(sizeof(GfxCubemap));

  cubemap->desc = {};
  cubemap->gfx  = gfx;

  glGenTextures(1, &cubemap->id);
  return cubemap;
}

const bool gfx_cubemap_load(GfxCubemap* cubemap, const GfxCubemapDesc& desc) {
  FREYA_DEBUG_ASSERT(cubemap, "Trying to load data into an invalid resource");
 
  cubemap->desc = desc;

  // Convert the GFX types into valid GL ones.

  GLenum in_format, gl_format, gl_pixel_type;
  gl_get_texture_format(desc.format, &in_format, &gl_format, &gl_pixel_type);

  GLenum gl_wrap_format = gl_get_texture_wrap(desc.wrap_mode);
  
  GLenum min_filter, mag_filter;
  gl_get_texture_filter(desc.filter, &min_filter, &mag_filter); 
 
  glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap->id);

  // Setting some parameters
  
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, min_filter);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, mag_filter);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, gl_wrap_format);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, gl_wrap_format);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, gl_wrap_format);
   
  glTexStorage2D(GL_TEXTURE_CUBE_MAP, desc.mips, in_format, desc.width, desc.height);

  // Set the texture for each face in the cubemap
  
  for(sizei i = 0; i < desc.faces_count; i++) {
    glTexSubImage3D(GL_TEXTURE_3D,               // Texture
                    0,                           // Levels
                    0, 0, i,                     // Offset (x, y, z)
                    desc.width, desc.height, 1,  // Size (width, height, depth)
                    gl_format,                   // Format
                    gl_pixel_type,               // Type
                    desc.data[i]);               // Pixels
  }
  
  return true;
}

void gfx_cubemap_destroy(GfxCubemap* cubemap, const FreeMemoryFn& free_fn) {
  if(!cubemap) {
    return;
  }
  
  glDeleteTextures(1, &cubemap->id);
  free_fn(cubemap);
}

GfxCubemapDesc& gfx_cubemap_get_desc(GfxCubemap* cubemap) {
  FREYA_DEBUG_ASSERT(cubemap, "Invalid GfxCubemap struct passed");
  return cubemap->desc;
}

void gfx_cubemap_update(GfxCubemap* cubemap, const GfxCubemapDesc& desc) {
  FREYA_DEBUG_ASSERT(cubemap->gfx, "Invalid GfxContext struct passed");
  FREYA_DEBUG_ASSERT(cubemap, "Invalid GfxCubemap struct passed");
  
  cubemap->desc = desc;
  
  // Updating the format
  
  GLenum in_format, gl_format, gl_pixel_type;
  gl_get_texture_format(desc.format, &in_format, &gl_format, &gl_pixel_type);

  // Updating the addressing mode
  GLenum gl_wrap_format = gl_get_texture_wrap(desc.wrap_mode);
  
  // Updating the filters
  
  GLenum min_filter, mag_filter;
  gl_get_texture_filter(desc.filter, &min_filter, &mag_filter); 
 
  glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap->id);

  // Re-setting the parameters
  
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, min_filter);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, mag_filter);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, gl_wrap_format);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, gl_wrap_format);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, gl_wrap_format);
}

void gfx_cubemap_upload_data(GfxCubemap* cubemap, 
                             const i32 width, const i32 height,
                             const void** faces, const sizei count) {
  FREYA_DEBUG_ASSERT(cubemap->gfx, "Invalid GfxContext struct passed in gfx_cubemap_upload_data");
  FREYA_DEBUG_ASSERT(cubemap, "Invalid GfxCubemap struct passed in gfx_cubemap_upload_data");
  FREYA_DEBUG_ASSERT(((count >= 0) && (count <= CUBEMAPS_MAX)), "The count parametar in gfx_cubemap_upload_data is invalid");
  FREYA_DEBUG_ASSERT(faces, "Invalid cubemap faces passed to gfx_cubemap_upload_data");
  
  // Updating the format
  
  GLenum in_format, gl_format, gl_pixel_type;
  gl_get_texture_format(cubemap->desc.format, &in_format, &gl_format, &gl_pixel_type);

  // Updating the information
  
  cubemap->desc.faces_count = count;
  cubemap->desc.width       = width;
  cubemap->desc.height      = height;

  // Updating the faces
 
  glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap->id);
  glTexStorage2D(GL_TEXTURE_CUBE_MAP, cubemap->desc.mips, in_format, width, height);

  for(sizei i = 0; i < count; i++) {
    cubemap->desc.data[i] = (void*)faces[i];
    glTexSubImage3D(GL_TEXTURE_3D,                                 // Texture
                    0,                                             // Levels 
                    0, 0, i,                                       // Offset (x, y, z)
                    cubemap->desc.width, cubemap->desc.height, 1,  // Size (width, height, depth)
                    gl_format,                                     // Format
                    gl_pixel_type,                                 // Type
                    faces[i]);                                     // Pixels
  }
}

///---------------------------------------------------------------------------------------------------------------------
/// Pipeline functions 

GfxPipeline* gfx_pipeline_create(GfxContext* gfx, const GfxPipelineDesc& desc, const AllocateMemoryFn& alloc_fn) {
  FREYA_DEBUG_ASSERT(gfx, "Invalid GfxContext struct passed");
  FREYA_DEBUG_ASSERT(desc.vertex_buffer, "Must have a vertex buffer to create a GfxPipeline struct");

  GfxPipeline* pipe = (GfxPipeline*)alloc_fn(sizeof(GfxPipeline));

  pipe->desc = desc;
  pipe->gfx  = gfx;

  // VAO init
  
  glGenVertexArrays(1, &pipe->vertex_array);
  glBindVertexArray(pipe->vertex_array);

  // Vertex buffer init
  
  pipe->vertex_buffer = desc.vertex_buffer; 
  pipe->vertex_count  = desc.vertices_count; 

  glBindBuffer(desc.vertex_buffer->gl_buff_type, desc.vertex_buffer->id);

  // Instance buffer init

  if(desc.instance_buffer) {
    pipe->instance_buffer = desc.instance_buffer; 
    pipe->instance_count  = desc.instance_count; 

    glBindBuffer(desc.instance_buffer->gl_buff_type, desc.instance_buffer->id);
  }

  // Index buffer init
  
  if(desc.index_buffer) {
    pipe->index_buffer = desc.index_buffer;
    pipe->index_count  = desc.indices_count;

    glBindBuffer(desc.index_buffer->gl_buff_type, desc.index_buffer->id);
  }

  // Pipeline layout init
 
  sizei strides[VERTEX_LAYOUTS_MAX];
  init_pipeline_layout(pipe, strides);

  // Set the draw mode for the whole pipeline
  pipe->draw_mode = desc.draw_mode;

  // Done!
  
  glBindVertexArray(0);
  return pipe;
}

void gfx_pipeline_destroy(GfxPipeline* pipeline, const FreeMemoryFn& free_fn) {
  FREYA_DEBUG_ASSERT(pipeline, "Attempting to free an invalid GfxPipeline");

  glDeleteVertexArrays(1, &pipeline->vertex_array);
  free_fn(pipeline);
}

void gfx_pipeline_update(GfxPipeline* pipeline, const GfxPipelineDesc& desc) {
  FREYA_DEBUG_ASSERT(pipeline, "Invalid GfxPipeline struct passed to gfx_pipeline_update");
  
  // Update the internal pipeline
  
  pipeline->desc = desc;

  pipeline->vertex_buffer = desc.vertex_buffer;
  pipeline->index_buffer  = desc.index_buffer;
  
  pipeline->vertex_count   = desc.vertices_count;
  pipeline->index_count    = desc.indices_count;
  pipeline->instance_count = desc.instance_count;
 
  pipeline->draw_mode = desc.draw_mode;
}

GfxPipelineDesc& gfx_pipeline_get_desc(GfxPipeline* pipeline) {
  FREYA_DEBUG_ASSERT(pipeline, "Invalid GfxPipeline struct passed");
  return pipeline->desc;
}

/// Pipeline functions 
///---------------------------------------------------------------------------------------------------------------------

} // End of freya

#endif // FREYA_GFX_GLES

//////////////////////////////////////////////////////////////////////////

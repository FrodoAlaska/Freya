#include "freya_render.h"
#include "freya_logger.h"

#include "shaders/batch_shader.h"

//////////////////////////////////////////////////////////////////////////

namespace freya { // Start of freya

///---------------------------------------------------------------------------------------------------------------------
/// Consts

const sizei VERTEX_BUFFER_SIZE         = MiB(1);
const sizei SHADER_MATRIX_BUFFER_INDEX = 0;

/// Consts
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// RenderBatch
struct RenderBatch {
  ShaderContext* shader;

  DynamicArray<GfxTexture*> textures;
  DynamicArray<Vertex2D> vertices;
};
/// RenderBatch
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Renderer
struct Renderer {
  //
  // Context
  //

  GfxContext* ctx         = nullptr;
  GfxContextDesc ctx_desc = {};

  //
  // Defaults
  //

  GfxBuffer* matrix_buffer;

  GfxPipelineDesc pipe_desc;
  GfxPipeline* pipeline;

  AssetID default_texture;

  //
  // Batching
  //

  HashMap<GfxTexture*, sizei> textures;
  RenderBatch quad_batch;

  Mat4 view        = Mat4(1.0f);
  Vec4 clear_color = Vec4(1.0f);
};

static Renderer s_renderer;
/// Renderer
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Private functions

static void batch_clear(RenderBatch& batch) {
  batch.textures.clear();
  batch.vertices.clear();
}

static void batch_generate_quad(RenderBatch& batch, const Rect2D& src, const Rect2D& dest, const Vec4& color, const sizei texture_index) {
  // Top-left
 
  Vertex2D v1 = {
    .position       = dest.position,
    .normal         = Vec2(0.0f, 1.0f),
    .texture_coords = src.position / src.size,

    .color         = color,
    .texture_index = (f32)texture_index,
  };
  batch.vertices.push_back(v1);

  // Top-right
  
  Vertex2D v2 = {
    .position       = Vec2(dest.position.x + dest.size.x, dest.position.y),
    .normal         = Vec2(1.0f, 0.0f),
    .texture_coords = Vec2((src.position.x + src.size.x) / src.size.x, src.position.y / src.size.y),

    .color          = color,
    .texture_index  = (f32)texture_index,
  };
  batch.vertices.push_back(v2);

  // Bottom-right
  
  Vertex2D v3 = {
    .position       = dest.position + dest.size,
    .normal         = Vec2(0.0f, -1.0f),
    .texture_coords = (src.position + src.size) / src.size,

    .color          = color,
    .texture_index  = (f32)texture_index,
  };
  batch.vertices.push_back(v3);
  batch.vertices.push_back(v3);

  // Bottom-left
  
  Vertex2D v4 = {
    .position       = Vec2(dest.position.x, dest.position.y + dest.size.y),
    .normal         = Vec2(-1.0f, 0.0f),
    .texture_coords = Vec2(src.position.x / src.size.x, (src.position.y + src.size.y) / src.size.y),

    .color          = color,
    .texture_index  = (f32)texture_index,
  };
  batch.vertices.push_back(v4);
  batch.vertices.push_back(v1);
}

static void batch_flush(RenderBatch& batch) {
  // Update the vertex buffer
  
  gfx_buffer_upload_data(s_renderer.pipe_desc.vertex_buffer, 
                         0, 
                         sizeof(Vertex2D) * batch.vertices.size(), 
                         batch.vertices.data());
  
  s_renderer.pipe_desc.vertices_count = batch.vertices.size();
  gfx_pipeline_update(s_renderer.pipeline, s_renderer.pipe_desc); 

  // Use the resources
  
  GfxBindingDesc bind_desc = {
    .shader = batch.shader->shader,

    .textures       = batch.textures.data(), 
    .textures_count = batch.textures.size(),
  };
  gfx_context_use_bindings(s_renderer.ctx, bind_desc);

  // Render the batch
  
  gfx_context_use_pipeline(s_renderer.ctx, s_renderer.pipeline); 
  gfx_context_draw(s_renderer.ctx, 0);

  // Start on a clean slate
  batch_clear(batch);
}

/// Private functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Renderer functions

void renderer_init(Window* window) {
  // Context init

  s_renderer.ctx_desc = {
    .window       = window,
    .states       = GFX_STATE_DEPTH | GFX_STATE_STENCIL | GFX_STATE_BLEND | GFX_STATE_MSAA,
    .has_vsync    = false,
  };

  s_renderer.ctx = gfx_context_init(s_renderer.ctx_desc);
  FREYA_ASSERT_LOG(s_renderer.ctx, "Failed to initialize the graphics context");
  
  //
  // Default matrix buffer init
  // 

  GfxBufferDesc buff_desc = {
    .data  = nullptr, 
    .size  = sizeof(Mat4),
    .type  = GFX_BUFFER_UNIFORM, 
    .usage = GFX_BUFFER_USAGE_DYNAMIC_DRAW,
  };
  s_renderer.matrix_buffer = asset_group_get_buffer(asset_group_push_buffer(ASSET_CACHE_ID, buff_desc));

  //
  // Default pipeline init
  // 

  // Vertex buffer init

  buff_desc = {
    .data  = nullptr, 
    .size  = VERTEX_BUFFER_SIZE,
    .type  = GFX_BUFFER_VERTEX, 
    .usage = GFX_BUFFER_USAGE_DYNAMIC_DRAW,
  };
  s_renderer.pipe_desc.vertex_buffer = asset_group_get_buffer(asset_group_push_buffer(ASSET_CACHE_ID, buff_desc));
  
  // Layout init
  
  s_renderer.pipe_desc.layouts[0].attributes[0]    = GFX_LAYOUT_FLOAT2; // Position 
  s_renderer.pipe_desc.layouts[0].attributes[1]    = GFX_LAYOUT_FLOAT2; // Normal
  s_renderer.pipe_desc.layouts[0].attributes[2]    = GFX_LAYOUT_FLOAT2; // Texture coords
  s_renderer.pipe_desc.layouts[0].attributes[3]    = GFX_LAYOUT_FLOAT4; // Color
  s_renderer.pipe_desc.layouts[0].attributes[4]    = GFX_LAYOUT_FLOAT1; // Texture index
  s_renderer.pipe_desc.layouts[0].attributes_count = 5;

  // Draw mode init 
  s_renderer.pipe_desc.draw_mode = GFX_DRAW_MODE_TRIANGLE;

  // Pipeline init
  s_renderer.pipeline = gfx_pipeline_create(s_renderer.ctx, s_renderer.pipe_desc);

  //
  // Default texture init
  //

  u32 pixels = 0xffffffff;

  GfxTextureDesc tex_desc;
  tex_desc.width  = 1;
  tex_desc.height = 1;
  tex_desc.type   = GFX_TEXTURE_2D;
  tex_desc.format = GFX_TEXTURE_FORMAT_RGBA8;
  tex_desc.data   = &pixels;

  s_renderer.default_texture = asset_group_push_texture(ASSET_CACHE_ID, tex_desc);

  //
  // Default shader init
  //

  AssetID shader_context_id    = asset_group_push_shader_context(ASSET_CACHE_ID, generate_batch_quad_shader());
  s_renderer.quad_batch.shader = asset_group_get_shader_context(shader_context_id); 

  // Set all of the values of the shader's texture array

  i32 texture_ids[TEXTURES_MAX];
  for(i32 i = 0; i < TEXTURES_MAX; i++) {
    texture_ids[i] = i;
  }

  shader_context_set_uniform_array(s_renderer.quad_batch.shader, "u_textures", texture_ids, TEXTURES_MAX);
  gfx_buffer_bind_point(s_renderer.matrix_buffer, SHADER_MATRIX_BUFFER_INDEX);

  //
  // Default framebuffer init
  //
  // @TODO (Renderer)

  // Done!
  FREYA_LOG_INFO("Successfully initialized the renderer context");
}

void renderer_shutdown() {
  // Destroy the batching stuff

  s_renderer.textures.clear();
  batch_clear(s_renderer.quad_batch);

  // Destroy the default pipeline
  gfx_pipeline_destroy(s_renderer.pipeline);

  // Destroy the context
  gfx_context_shutdown(s_renderer.ctx);

  // Done!
  FREYA_LOG_INFO("Successfully shutdown the renderer context");
}

void renderer_begin(const Camera& camera) {
  FREYA_PROFILE_FUNCTION();
 
  // Update the camera projection

  IVec2 window_size = window_get_size(s_renderer.ctx_desc.window);

  Mat4 ortho = mat4_ortho(0.0f, (f32)window_size.x, (f32)window_size.y, 0.0f);
  Mat4 view  = ortho * camera.transform.model;

  // Update the camera buffer
  
  gfx_buffer_upload_data(s_renderer.matrix_buffer, 
                         0, 
                         sizeof(Mat4), 
                         mat4_raw_data(view));

  // @TODO (Renderer): Set the renderer's framebuffer

  gfx_context_set_target(s_renderer.ctx, nullptr);

  Vec4& col = s_renderer.clear_color;
  gfx_context_clear(s_renderer.ctx, col.r, col.g, col.b, col.a);

  // Clean the slate!

  s_renderer.textures.clear();
  batch_clear(s_renderer.quad_batch);
}

void renderer_end() {
  FREYA_PROFILE_FUNCTION();

  // Flush the quad batch
  batch_flush(s_renderer.quad_batch);
}

void renderer_set_clear_color(const Vec4& color) {
  s_renderer.clear_color = color;
}

const Vec4& renderer_get_clear_color() {
  return s_renderer.clear_color;
}

GfxContext* renderer_get_context() {
  return s_renderer.ctx;
}

void renderer_queue_texture(const AssetID& texture_id, const Rect2D& src, const Rect2D& dest, const Vec4& tint) {
  // Get the pure texture representation
  
  GfxTexture* texture = asset_group_get_texture(texture_id);
  sizei index         = 0;

  // Save the texture in the map if it never existed before
  
  if(s_renderer.textures.find(texture) == s_renderer.textures.end()) {
    sizei index = s_renderer.quad_batch.textures.size();
    s_renderer.quad_batch.textures.push_back(texture);
    
    s_renderer.textures[texture] = index;
  }
  else { // Otherwise, retrieve the relevant index
    index = s_renderer.textures[texture];
  }

  // Make sure we flush the batch if it exceeds any limits
  
  if(s_renderer.quad_batch.textures.size() >= TEXTURES_MAX) {
    batch_flush(s_renderer.quad_batch);
  }

  // Generate a quad 
  batch_generate_quad(s_renderer.quad_batch, src, dest, tint, index);  
}

void renderer_queue_texture(const AssetID& texture_id, const Transform& transform, const Vec4& tint) {
  Rect2D src = {
    .size     = transform.scale,
    .position = Vec2(0.0f),
  };
  
  Rect2D dest = {
    .size     = transform.scale,
    .position = transform.position, 
  };

  renderer_queue_texture(texture_id, src, dest, tint);  
}

void renderer_queue_quad(const Transform& transform, const Vec4& color) {
  renderer_queue_texture(s_renderer.default_texture, transform, color);
}

/// Renderer functions
///---------------------------------------------------------------------------------------------------------------------

} // End of freya

//////////////////////////////////////////////////////////////////////////

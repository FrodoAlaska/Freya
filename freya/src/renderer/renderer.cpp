#include "freya_render.h"
#include "freya_logger.h"

#include "shaders/batch_shaders.h"

//////////////////////////////////////////////////////////////////////////

namespace freya { // Start of freya

///---------------------------------------------------------------------------------------------------------------------
/// Consts

const sizei VERTEX_BUFFER_SIZE         = MiB(1);
const sizei SHADER_MATRIX_BUFFER_INDEX = 0;

/// Consts
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// ShaderID
enum ShaderID {
  SHADER_BATCH = 0, 
  SHADER_QUAD, 
  SHADER_CIRCLE,
  SHADER_POLYGON,

  SHADERS_MAX,
};
/// ShaderID
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// RenderBatch
struct RenderBatch {
  ShaderContext* shader;

  GfxTexture* texture;
  DynamicArray<Vertex2D> vertices;
};
/// RenderBatch
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// DebugBatch
struct DebugBatch {
  ShaderID shader_id;

  // Common uniforms

  Mat4 model;
  Color color;

  // Uniforms that are specific to shaders
  
  f32 radius;
  i32 sides;
};
/// DebugBatch
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
  
  GfxPipelineDesc debug_pipe_desc;
  GfxPipeline* debug_pipeline;

  GfxTexture* default_texture;

  ShaderContext* shaders[SHADERS_MAX];

  //
  // Batching
  //

  HashMap<GfxTexture*, sizei> textures;

  DynamicArray<RenderBatch> batches;
  DynamicArray<DebugBatch> debug_batches;

  Color clear_color = Color(1.0f);
};

static Renderer s_renderer;
/// Renderer
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Private functions

static void batch_clear(RenderBatch& batch) {
  // @TODO: Possibly other things??
  batch.vertices.clear();
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

    .textures       = &batch.texture, 
    .textures_count = 1,
  };
  gfx_context_use_bindings(s_renderer.ctx, bind_desc);

  // Render the batch
  
  gfx_context_use_pipeline(s_renderer.ctx, s_renderer.pipeline); 
  gfx_context_draw(s_renderer.ctx, 0);

  // Start on a clean slate
  batch_clear(batch);
}

static void batches_flush_all() {
  for(auto& batch : s_renderer.batches) {
    batch_flush(batch);
  }

  s_renderer.textures.clear();
  s_renderer.batches.clear();
}

static RenderBatch& batch_find(GfxTexture* texture) {
  sizei index = 0;

  // Make sure we flush the batch if it exceeds any limits
  
  if(s_renderer.textures.size() >= TEXTURES_MAX) {
    batches_flush_all();
  }

  // Save the texture in the set if it never existed before
  
  if(!s_renderer.textures.contains(texture)) {
    index = s_renderer.batches.size();
    s_renderer.batches.emplace_back(s_renderer.shaders[SHADER_BATCH], texture);
    
    s_renderer.textures[texture] = index;
  }
  else { // Otherwise, retrieve the relevant index
    index = s_renderer.textures[texture];
  }

  // Done!
  return s_renderer.batches[index];
}

static void batch_generate_quad(RenderBatch& batch, 
                                const Rect2D& src, 
                                const Rect2D& dest, 
                                const f32 rotation, 
                                const Color& color) {
  ///  
  /// @NOTE:
  /// Most of this code came from the wonderful Raylib library. 
  /// In particular, it was taken from here: https://github.com/raysan5/raylib/blob/master/src/rtextures.c
  ///

  Vec2 center     = dest.size / 2.0f;
  Vec2 origin_pos = dest.position - center;

  // Set the default destination rects

  Vec2 top_left     = origin_pos;
  Vec2 bottom_left  = Vec2(origin_pos.x, origin_pos.y + dest.size.y);
  Vec2 bottom_right = origin_pos + dest.size;
  Vec2 top_right    = Vec2(origin_pos.x + dest.size.x, origin_pos.y);

  // Calculate the rotation of each vertex (if there is any rotation)
  
  if(rotation != 0.0f) {
    // Calculate the sine and cosine

    f32 sin_rot = freya::sin(rotation);
    f32 cos_rot = freya::cos(rotation);

    Vec2 origin = -center;

    // Rotate each corner

    top_left.x = (dest.position.x + origin.x * cos_rot) - (origin.y * sin_rot);
    top_left.y = (dest.position.y + origin.x * sin_rot) + (origin.y * cos_rot);

    bottom_left.x = (dest.position.x + origin.x * cos_rot) - ((origin.y + dest.size.y) * sin_rot);
    bottom_left.y = (dest.position.y + origin.x * sin_rot) + ((origin.y + dest.size.y) * cos_rot);

    bottom_right.x = (dest.position.x + (origin.x + dest.size.x) * cos_rot) - ((origin.y + dest.size.y) * sin_rot);
    bottom_right.y = (dest.position.y + (origin.x + dest.size.x) * sin_rot) + ((origin.y + dest.size.y) * cos_rot);

    top_right.x = (dest.position.x + (origin.x + dest.size.x) * cos_rot) - (origin.y * sin_rot);
    top_right.y = (dest.position.y + (origin.x + dest.size.x) * sin_rot) + (origin.y * cos_rot);
  }

  // Get the size of the texture
  
  GfxTextureDesc& tex_desc = gfx_texture_get_desc(batch.texture);
  Vec2 texture_size        = Vec2(tex_desc.width, tex_desc.height);

  // Top-left
 
  Vertex2D v1 = {
    .position       = top_left,
    .normal         = Vec2(0.0f, 1.0f),
    .texture_coords = src.position / texture_size,
    .color          = color,
  };
  batch.vertices.push_back(v1);

  // Bottom-left
  
  Vertex2D v2 = {
    .position       = bottom_left,
    .normal         = Vec2(-1.0f, 0.0f),
    .texture_coords = Vec2(src.position.x / texture_size.x, (src.position.y + src.size.y) / texture_size.y),
    .color          = color,
  };
  batch.vertices.push_back(v2);

  // Bottom-right
  
  Vertex2D v3 = {
    .position       = bottom_right,
    .normal         = Vec2(0.0f, -1.0f),
    .texture_coords = (src.position + src.size) / texture_size,
    .color          = color,
  };
  batch.vertices.push_back(v3);
  batch.vertices.push_back(v3);

  // Top-right
  
  Vertex2D v4 = {
    .position       = top_right,
    .normal         = Vec2(1.0f, 0.0f),
    .texture_coords = Vec2((src.position.x + src.size.x) / texture_size.x, src.position.y / texture_size.y),
    .color          = color,
  };
  batch.vertices.push_back(v4);
  batch.vertices.push_back(v1);
}

/// Private functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Renderer functions

void renderer_init(Window* window) {
  // Context init

  s_renderer.ctx_desc = {
    .window    = window,
    .states    = GFX_STATE_DEPTH | GFX_STATE_BLEND | GFX_STATE_MSAA,
    .has_vsync = false,
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
  s_renderer.pipe_desc.layouts[0].attributes_count = 4;

  // Draw mode init 
  s_renderer.pipe_desc.draw_mode = GFX_DRAW_MODE_TRIANGLE;

  // Pipeline init
  s_renderer.pipeline = gfx_pipeline_create(s_renderer.ctx, s_renderer.pipe_desc);

  //
  // Debug pipeline init
  //

  // Vertex buffer init

  f32 vertices[] = {
    -0.5f, -0.5f, 0.0f, 0.0f,
     0.5f, -0.5f, 1.0f, 0.0f,
     0.5f,  0.5f, 1.0f, 1.0f,
    
     0.5f,  0.5f, 1.0f, 1.0f,
    -0.5f,  0.5f, 0.0f, 1.0f,
    -0.5f, -0.5f, 0.0f, 0.0f,
  };

  buff_desc = {
    .data  = vertices, 
    .size  = sizeof(vertices),
    .type  = GFX_BUFFER_VERTEX, 
    .usage = GFX_BUFFER_USAGE_STATIC_DRAW,
  };

  s_renderer.debug_pipe_desc.vertex_buffer  = asset_group_get_buffer(asset_group_push_buffer(ASSET_CACHE_ID, buff_desc));
  s_renderer.debug_pipe_desc.vertices_count = 6;
  
  // Layout init
  
  s_renderer.debug_pipe_desc.layouts[0].attributes[0]    = GFX_LAYOUT_FLOAT2; // Position 
  s_renderer.debug_pipe_desc.layouts[0].attributes[1]    = GFX_LAYOUT_FLOAT2; // Texture coords
  s_renderer.debug_pipe_desc.layouts[0].attributes_count = 2;

  // Draw mode init 
  s_renderer.debug_pipe_desc.draw_mode = GFX_DRAW_MODE_TRIANGLE;

  // Pipeline init
  s_renderer.debug_pipeline = gfx_pipeline_create(s_renderer.ctx, s_renderer.debug_pipe_desc);

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

  s_renderer.default_texture = asset_group_get_texture(asset_group_push_texture(ASSET_CACHE_ID, tex_desc));

  //
  // Default shaders init
  //

  // Set the matrix buffer point
  gfx_buffer_bind_point(s_renderer.matrix_buffer, SHADER_MATRIX_BUFFER_INDEX);

  //
  // Debug shaders init
  //  

  AssetID shader_ids[SHADERS_MAX] = {
    asset_group_push_shader(ASSET_CACHE_ID, generate_batch_quad_shader()),
    asset_group_push_shader(ASSET_CACHE_ID, generate_quad_shader()),
    asset_group_push_shader(ASSET_CACHE_ID, generate_circle_shader()),
    asset_group_push_shader(ASSET_CACHE_ID, generate_polygon_shader()),
  };

  for(sizei i = 0; i < SHADERS_MAX; i++) {
    s_renderer.shaders[i] = asset_group_get_shader_context(asset_group_push_shader_context(ASSET_CACHE_ID, shader_ids[i]));
  }

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

  for(auto& batch : s_renderer.batches) {
    batch_clear(batch);
  }
  s_renderer.batches.clear();

  // Destroy the default pipelines
  
  gfx_pipeline_destroy(s_renderer.pipeline);
  gfx_pipeline_destroy(s_renderer.debug_pipeline);

  // Destroy the context
  gfx_context_shutdown(s_renderer.ctx);

  // Done!
  FREYA_LOG_INFO("Successfully shutdown the renderer context");
}

void renderer_begin(Camera& camera) {
  FREYA_PROFILE_FUNCTION();
 
  // Update the camera projection

  Transform camera_transform = {
    .position = -camera.position, 
    .scale    = Vec2(camera.zoom), 
    .rotation = camera.rotation
  };

  IVec2 window_size = window_get_size(s_renderer.ctx_desc.window);

  camera.view       = mat4_transform(camera_transform);
  camera.projection = mat4_ortho(0.0f, (f32)window_size.x, (f32)window_size.y, 0.0f);
  camera.view_proj  = camera.projection * camera.view;

  // Update the camera buffer
  
  gfx_buffer_upload_data(s_renderer.matrix_buffer, 
                         0, 
                         sizeof(Mat4), 
                         mat4_raw_data(camera.view_proj));

  // @TODO (Renderer/post-process): Set the renderer's framebuffer

  gfx_context_set_target(s_renderer.ctx, nullptr);

  Color& col = s_renderer.clear_color;
  gfx_context_clear(s_renderer.ctx, col.r, col.g, col.b, col.a);

  // Clean the slate!
 
  s_renderer.textures.clear();
  s_renderer.batches.clear();
  s_renderer.debug_batches.clear();
}

void renderer_end() {
  FREYA_PROFILE_FUNCTION();

  // Flush the batches
  batches_flush_all();

  // Flush the debug batches

  for(auto& batch : s_renderer.debug_batches) {
    // Get the corresponding shader
    ShaderContext* shader = s_renderer.shaders[batch.shader_id];

    // Set the common uniforms 
  
    shader_context_set_uniform(shader, "u_color", batch.color);
    shader_context_set_uniform(shader, "u_model", batch.model);

    // Set uniform that are specific to the shader
 
    switch(batch.shader_id) {
      case SHADER_QUAD:
      case SHADER_CIRCLE:
        break;
      case SHADER_POLYGON:
        shader_context_set_uniform(shader, "u_radius", batch.radius);
        shader_context_set_uniform(shader, "u_sides", batch.sides);
        break;
      default:
        break;
    }

    // Use the resources

    GfxBindingDesc bind_desc = {
      .shader = shader->shader,
    };
    gfx_context_use_bindings(s_renderer.ctx, bind_desc);

    // Render the batch

    gfx_context_use_pipeline(s_renderer.ctx, s_renderer.debug_pipeline); 
    gfx_context_draw(s_renderer.ctx, 0);
  }
}

void renderer_set_clear_color(const Color& color) {
  s_renderer.clear_color = color;
}

const Color& renderer_get_clear_color() {
  return s_renderer.clear_color;
}

GfxContext* renderer_get_context() {
  return s_renderer.ctx;
}

void renderer_queue_texture(GfxTexture* texture, const Rect2D& src, const Rect2D& dest, const f32 rotation, const Color& tint) {
  // Get the appropriate batch based on the given texture
  RenderBatch& batch = batch_find(texture);

  // Generate a quad 
  batch_generate_quad(batch, src, dest, rotation, tint); 
}

void renderer_queue_texture(GfxTexture* texture, const Transform& transform, const Color& tint) {
  GfxTextureDesc& tex_desc = gfx_texture_get_desc(texture);

  Rect2D src = {
    .size     = Vec2(tex_desc.width, tex_desc.height),
    .position = Vec2(0.0f),
  };
  
  Rect2D dest = {
    .size     = Vec2(tex_desc.width, tex_desc.height) * transform.scale,
    .position = transform.position, 
  };

  renderer_queue_texture(texture, src, dest, transform.rotation, tint);  
}

void renderer_queue_quad(const Transform& transform, const Color& color) {
  renderer_queue_texture(s_renderer.default_texture, transform, color);
}

void renderer_queue_animation(const Animation& anim, const Transform& transform, const Color& tint) {
  GfxTextureDesc& tex_desc = gfx_texture_get_desc(anim.texture);
  
  Rect2D dest = {
    .size     = anim.frame_size * transform.scale,
    .position = transform.position, 
  };
  renderer_queue_texture(anim.texture, anim.src_rect, dest, transform.rotation, tint);
}

void renderer_queue_particles(const ParticleEmitter& emitter) {
  if(!emitter.is_active) {
    return;
  }

  for(sizei i = 0; i < emitter.particles_count; i++) {
    if(emitter.texture) {
      renderer_queue_texture(emitter.texture, emitter.transforms[i], emitter.color);
      continue;
    }

    renderer_queue_quad(emitter.transforms[i], emitter.color);
  }
}

void renderer_draw_debug_quad(const Vec2& position, const Vec2& size, const f32 rotation, const Color& color) {
  // Construct the transform

  Transform transform = {
    .position = position, 
    .scale    = size,
    .rotation = rotation,
  };
  
  // Add to the batch

  s_renderer.debug_batches.emplace_back(SHADER_QUAD, 
                                        mat4_transform(transform), 
                                        color, 
                                        0.0f, 
                                        0);
}

void renderer_draw_debug_circle(const Vec2& position, const f32 radius, const Color& color) {
  // Construct the transform

  Transform transform = {
    .position = position, 
    .scale    = Vec2(radius),
  };
  
  // Add to the batch

  s_renderer.debug_batches.emplace_back(SHADER_CIRCLE, 
                                        mat4_transform(transform), 
                                        color, 
                                        radius, 
                                        0);
}

void renderer_draw_debug_polygon(const Vec2& center, const f32 radius, const i32 sides, const Color& color) {
  // Construct the transform

  Transform transform = {
    .position = center, 
    .scale    = Vec2(radius),
  };
  
  // Add to the batch

  s_renderer.debug_batches.emplace_back(SHADER_POLYGON, 
                                        mat4_transform(transform), 
                                        color, 
                                        radius, 
                                        sides);
}

/// Renderer functions
///---------------------------------------------------------------------------------------------------------------------

} // End of freya

//////////////////////////////////////////////////////////////////////////

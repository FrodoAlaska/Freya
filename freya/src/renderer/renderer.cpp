#include "freya_render.h"
#include "freya_logger.h"
#include "freya_event.h"

#include "shaders/default_pass_shader.h"
#include "ui/ui_renderer_impl.h" 

#include "sokol/sokol_gp.h"

//////////////////////////////////////////////////////////////////////////

namespace freya { // Start of freya

///---------------------------------------------------------------------------------------------------------------------
/// Renderer
struct Renderer {
  Window* window = nullptr;

  sg_sampler default_sampler;
  sg_buffer vertex_buffer;

  Color color = Color(1.0f);
  DynamicArray<PostProcessPass*> passes;

  sg_pass_action pass_action;
  sg_pass pass;

  sg_pipeline pipeline;
  DynamicArray<UIContext*> ui_contexts;

  AssetGroupID group_id = ASSET_CACHE_ID;
};

static Renderer s_renderer;
/// Renderer
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Callbacks

static bool window_resized_callback(const Event& event, const void* dispatcher, const void* listener) {
  // Resize each render pass
  
  IVec2 new_size = IVec2(event.window_framebuffer_width, event.window_framebuffer_height);

  for(PostProcessPass* pass : s_renderer.passes) {
    if(!pass->resize_func) {
      continue;
    }
    
    pass->resize_func(pass, new_size);
  }

  // Done!
  return true;
}

static void sg_logger_func(const char* tag, 
                           u32 level, 
                           u32 item, 
                           const char* msg, 
                           u32 line, 
                           const char* filename, 
                           void* user_data) {
  switch(level) {
    case 0: // Panic
      FREYA_LOG_FATAL("%s - (%i, %s)", msg, line, filename); 
      FREYA_ASSERT_LOG(false, "Fatal SOKOL error encountered");
      break;
    case 1: // Error
      FREYA_LOG_ERROR("[SOKOL-ERROR]: %s - (%i, %s)", msg, line, filename); 
      break;
    case 2: // Warning
      FREYA_LOG_WARN("[SOKOL-WARN]: %s - (%i, %s)", msg, line, filename); 
      break;
    case 3: // Info
      FREYA_LOG_INFO("[SOKOL-INFO]: %s - (%i, %s)", msg, line, filename); 
      break;
  }
}

/// Callbacks
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Private functions

static void swapchain_pass_prepare() {
  // Set up the swapchain 

  s_renderer.pass_action.colors[0].clear_value = {
    .r = s_renderer.color.r,
    .g = s_renderer.color.g,
    .b = s_renderer.color.b,
    .a = s_renderer.color.a,
  }; 

  s_renderer.pass.action    = s_renderer.pass_action;
  s_renderer.pass.swapchain = renderer_get_default_swapchain();

  // Begin the pass
  sg_begin_pass(&s_renderer.pass);
}

static void swapchain_pass_commit(PostProcessPass* current_pass) {
  if(current_pass) {
    // Set up the bindings of the pass 
    sg_bindings bindings = {};

    // Use the final texture image of the current pass
    
    bindings.views[0] = current_pass->outputs[0];

    bindings.vertex_buffers[0] = s_renderer.vertex_buffer;
    bindings.samplers[0]       = s_renderer.default_sampler;

    // Apply the bindings and the pipeline

    sg_apply_pipeline(s_renderer.pipeline);
    sg_apply_bindings(bindings);

    // Render the screen-space post-process effect
    sg_draw(0, 6, 1);
  }

  // End the swapchain pass
  sg_end_pass();
}

/// Private functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Renderer functions

void renderer_init(Window* window) {
  s_renderer.window = window;

  // GFX init

  sg_desc gfx_desc     = {};
  gfx_desc.logger.func = sg_logger_func;

  gfx_desc.environment.defaults = sg_environment_defaults {
    .color_format = SG_PIXELFORMAT_RGBA8,
    .depth_format = SG_PIXELFORMAT_DEPTH_STENCIL,
    .sample_count = window_get_samples_count(s_renderer.window),
  };

  sg_setup(&gfx_desc);
  FREYA_ASSERT_LOG(sg_isvalid(), "Failed to initialize the graphics context");

  // SGP init

  sgp_desc gp_desc = {};
  sgp_setup(&gp_desc);
  
  FREYA_ASSERT_LOG(sgp_is_valid(), "Failed to initialize the graphics painter");

  // Default sampler init

  sg_sampler_desc sampler_desc = {};
  s_renderer.default_sampler   = sg_make_sampler(sampler_desc);

  // Vertex buffer init

  f32 vertices[] = {
    -1.0f, -1.0f, 0.0f, 0.0f,
     1.0f, -1.0f, 1.0f, 0.0f,
     1.0f,  1.0f, 1.0f, 1.0f,
    
     1.0f,  1.0f, 1.0f, 1.0f,
    -1.0f,  1.0f, 0.0f, 1.0f,
    -1.0f, -1.0f, 0.0f, 0.0f,
  };

  sg_buffer_desc buff_desc = {
    .data = SG_RANGE(vertices),
  };
  s_renderer.vertex_buffer = sg_make_buffer(buff_desc);

  // Default pass init
  
  PostProcessPassDesc pass_desc = {
    .frame_size    = window_get_size(s_renderer.window),
    .clear_color   = Color(0.1f, 0.1f, 0.1f, 1.0f),
    .shader_id     = asset_group_push_shader(s_renderer.group_id, *default_pass_shader_desc(sg_query_backend())),
    .group_id      = s_renderer.group_id,
    .samples_count = window_get_samples_count(s_renderer.window),
    .debug_name    = "Default",
  };

  pass_desc.attachments.emplace_back(SG_PIXELFORMAT_RGBA8);
  pass_desc.attachments.emplace_back(SG_PIXELFORMAT_DEPTH_STENCIL);

  PostProcessPass* default_pass = post_process_create(s_renderer.window, pass_desc);
  renderer_push_post_process(default_pass);

  default_pass->outputs[0]    = default_pass->attachments[0];
  default_pass->outputs_count = 1;
 
  // Default pipeline init 
  
  sg_pipeline_desc pipe_desc = {};
  
  pipe_desc.depth.compare       = SG_COMPAREFUNC_LESS_EQUAL;
  pipe_desc.depth.write_enabled = true;

  pipe_desc.shader = asset_group_get_shader(pass_desc.shader_id); // Same shader as the default post-process pass

  pipe_desc.layout.attrs[0].format = SG_VERTEXFORMAT_FLOAT2; // Position
  pipe_desc.layout.attrs[1].format = SG_VERTEXFORMAT_FLOAT2; // Texture coords

  s_renderer.pipeline = sg_make_pipeline(pipe_desc);

  // Listen to events
  
  event_register(EVENT_WINDOW_MAXIMIZED, window_resized_callback);
  event_register(EVENT_WINDOW_FRAMEBUFFER_RESIZED, window_resized_callback);
  event_register(EVENT_WINDOW_FULLSCREEN, window_resized_callback);

  // UI renderer init
  ui_renderer_init(s_renderer.window); 

  // Done!
  FREYA_LOG_INFO("Successfully initialized the renderer context");
}

void renderer_shutdown() {
  // Destroy the framebuffers

  for(PostProcessPass* pass : s_renderer.passes) {
    post_process_destroy(pass);
  }
  s_renderer.passes.clear();

  // UI renderer shutdown
  ui_renderer_shutdown();

  // GFX shutdown

  sgp_shutdown();
  sg_shutdown();

  // Done!
  FREYA_LOG_INFO("Successfully shutdown the renderer context");
}

void renderer_apply_asset_group(const AssetGroupID& group_id) {
  s_renderer.group_id = group_id;
}

bool renderer_apply_font(const AssetID& font_id) {
  return ui_renderer_apply_font(font_id);
}

void renderer_begin(Camera& camera) {
  FREYA_PROFILE_FUNCTION();

  // Reset the renderer's state 

  IVec2 frame_size = window_get_framebuffer_size(s_renderer.window);
  IVec2 size       = window_get_size(s_renderer.window);

  // SGP begin

  sgp_begin(size.x, size.y);
  sgp_project(0.0f, (f32)camera.view_bounds.x, 0.0f, (f32)camera.view_bounds.y);
  sgp_set_blend_mode(SGP_BLENDMODE_BLEND);

  // Pusing the camera transformation

  sgp_push_transform();

  sgp_translate(camera.position.x, camera.position.y);
  sgp_rotate(camera.rotation);
  sgp_scale(camera.zoom, camera.zoom);

  // Clear

  sgp_set_color(s_renderer.color.r, s_renderer.color.g, s_renderer.color.b, s_renderer.color.a);
  sgp_clear();

  // Either start the post-processing chain if there are 
  // more than the default pass available, or just prepare 
  // the default swapchain for normal rendering.
  
  if(s_renderer.passes.size() > 1) {
    PostProcessPass* pass = s_renderer.passes[0];
    sg_begin_pass(&pass->pass);
    sg_apply_viewport(0, 0, pass->frame_size.x, pass->frame_size.y, true);
  }
  else {
    swapchain_pass_prepare();
  }
}

void renderer_end() {
  FREYA_PROFILE_FUNCTION();
  
  // Reset the painter's state
  
  sgp_pop_transform();
  sgp_reset_blend_mode();

  // End the painter 
  
  sgp_flush();
  sgp_end();

  // Render the contexts

  ui_renderer_prepare();
  for(auto& ctx : s_renderer.ui_contexts) {
    ui_context_render(ctx);
  }
  s_renderer.ui_contexts.clear();

  // End the default post-processing pass if we are 
  // currently expected to walk that chain 

  if(s_renderer.passes.size() > 1) {
    sg_end_pass(); // Should be the default post-process pass
  }

  // Initiate the post-processing pipeline

  PostProcessPass* current_pass = nullptr;
  for(sizei i = 1; i < s_renderer.passes.size(); i++) { // Skip the default pass
    // Prepare the pass
    
    PostProcessPass* pass = s_renderer.passes[i];
    sg_begin_pass(&pass->pass);
    sg_apply_viewport(0, 0, pass->frame_size.x, pass->frame_size.y, true);

    // Set up the bindings of the pass 
    sg_bindings bindings = {};

    // Use the outputs of the previous pass
     
    PostProcessPass* previous = pass->previous;
    for(u32 i = 0; i < previous->outputs_count; i++) {
      bindings.views[i] = previous->outputs[i];
    }

    bindings.vertex_buffers[0] = s_renderer.vertex_buffer;
    bindings.samplers[0]       = s_renderer.default_sampler;

    // Apply the bindings and the pipeline
    
    sg_apply_pipeline(pass->pipeline);
    sg_apply_bindings(bindings);

    if(pass->prepare_func) {
      pass->prepare_func(pass);
    }

    // Render the screen-space post-process effect
    sg_draw(0, 6, 1);

    // End the pass
    sg_end_pass();

    // Set the current pass to render later
    current_pass = pass; 
  }

  // Only preapre the default swapchain if we have some 
  // post-processing passes to go through. If there aren't any, 
  // then the default swapchain was already prepared at the start of the frame.

  if(current_pass) {
    swapchain_pass_prepare();
  }

  // Draw the final image to the swapchain
  swapchain_pass_commit(current_pass);

  // Done with this frame... 
  sg_commit();
}

void renderer_push_post_process(PostProcessPass* pass) {
  FREYA_DEBUG_ASSERT(pass, "");

  // Push the pass to the back and set its previous

  if(!s_renderer.passes.empty()) {
    pass->previous = s_renderer.passes.back();
  }

  s_renderer.passes.push_back(pass);

  // Some useful info
  FREYA_LOG_TRACE("Pushed post-process \'%s\' to the chain", pass->debug_name.c_str());
}

PostProcessPass* renderer_pop_post_process() {
  // We only have the default pass left, so do not pop anymore

  if(s_renderer.passes.size() == 1) {
    FREYA_LOG_TRACE("Could not pop anymore passes");
    return nullptr;
  }

  // Pop the pass, otherwise

  PostProcessPass* pass = s_renderer.passes.back();
  s_renderer.passes.pop_back();

  // Done!

  FREYA_LOG_TRACE("Popped post-process \'%s\' from the chain", pass->debug_name.c_str());
  return pass;
}

void renderer_set_clear_color(const Color& color) {
  s_renderer.color = color;
}

const Color& renderer_get_clear_color() {
  return s_renderer.color;
}

u32 renderer_get_post_process_count() {
  return (u32)s_renderer.passes.size();
}

sg_swapchain renderer_get_default_swapchain() {
  sg_swapchain swapchain = {};

  IVec2 frame_size = window_get_framebuffer_size(s_renderer.window);

  swapchain.width  = frame_size.x;
  swapchain.height = frame_size.y;

  swapchain.sample_count   = window_get_samples_count(s_renderer.window);
  swapchain.gl.framebuffer = 0; // The default framebuffer in GL is always 0

  return swapchain;
}

AssetGroupID& renderer_get_asset_group_id() {
  return s_renderer.group_id;
}

void renderer_queue_texture(const Texture& texture, 
                            const Rect2D& src, 
                            const Rect2D& dest, 
                            const f32 rotation,
                            const Color& tint) {
  sgp_set_color(tint.r, tint.g, tint.b, tint.a);
  sgp_rotate(rotation);

  Vec2 src_pos  = src.position;
  Vec2 src_size = src.size;

  Vec2 dest_pos  = dest.position;
  Vec2 dest_size = dest.size; 

  sgp_set_view(0, texture.view);
  sgp_set_sampler(0, texture.sampler);

  sgp_draw_textured_rect(0, 
                         {dest_pos.x, dest_pos.y, dest_size.x, dest_size.y},
                         {src_pos.x, src_pos.y, src_size.x, src_size.y}); 
}

void renderer_queue_texture(const Texture& texture, const Transform& transform, const Color& tint) {
  Rect2D src = {
    .size     = texture.size,
    .position = Vec2(0.0f),
  };
  
  Rect2D dest = {
    .size     = transform.scale,
    .position = transform.position, 
  };

  renderer_queue_texture(texture, src, dest, transform.rotation, tint);
}

void renderer_queue_quad(const Transform& transform, const Color& color) {
  sgp_set_color(color.r, color.g, color.b, color.a);

  sgp_rotate(transform.rotation);
  sgp_draw_filled_rect(transform.position.x, transform.position.y, transform.scale.x, transform.scale.y);
}

void renderer_queue_line(const Vec2& start, const Vec2& end, const Color& color) {
  sgp_set_color(color.r, color.g, color.b, color.a);
  sgp_draw_line(start.x, start.y, end.x, end.y);
}

void renderer_queue_point(const Vec2& position, f32 size, const Color& color) {
  sgp_set_color(color.r, color.g, color.b, color.a);
  sgp_scale(size, size);

  sgp_draw_point(position.x, position.y);
}

void renderer_queue_triangle(const Vec2& p1, const Vec2& p2, const Vec2& p3, const Color& color) {
  sgp_set_color(color.r, color.g, color.b, color.a);
  sgp_draw_filled_triangle(p1.x, p1.y, p2.x, p2.y, p3.x, p3.y);
}

void renderer_queue_triangles_strip(const Transform& transform, const DynamicArray<Vec2>& vertices, const Color& color) {
  sgp_set_color(color.r, color.g, color.b, color.a);

  sgp_push_transform();

  sgp_translate(transform.position.x, transform.position.y);
  sgp_rotate(transform.rotation);
  sgp_scale(transform.scale.x, transform.scale.y);

  // @TEMP (Renderer)

  DynamicArray<sgp_point> sgp_points(vertices.size());
  for(sizei i = 0; i < sgp_points.size(); i++) {
    sgp_points[i] = sgp_point{vertices[i].x, vertices[i].y};
  }

  sgp_draw_filled_triangles_strip(sgp_points.data(), vertices.size());

  sgp_pop_transform();
}

void renderer_queue_animation(const Animation& anim, const Transform& transform, const Color& tint) {
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
    if(emitter.texture.id != -1) {
      renderer_queue_texture(emitter.texture, emitter.transforms[i], emitter.color);
      continue;
    }

    renderer_queue_quad(emitter.transforms[i], emitter.color);
  }
}

void renderer_queue_ui_context(UIContext* ui_ctx) {
  s_renderer.ui_contexts.emplace_back(ui_ctx);
}

/// Renderer functions
///---------------------------------------------------------------------------------------------------------------------

} // End of freya

//////////////////////////////////////////////////////////////////////////

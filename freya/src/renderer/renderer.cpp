#include "freya_render.h"
#include "freya_logger.h"
#include "freya_event.h"

#include "sokol/sokol_gp.h"

//////////////////////////////////////////////////////////////////////////

namespace freya { // Start of freya

///---------------------------------------------------------------------------------------------------------------------
/// Renderer
struct Renderer {
  Window* window = nullptr;

  sg_pass pass               = {};
  sg_pass_action pass_action = {};

  Color color = Color(1.0f);
  
  PostProcessPass* default_pass = nullptr;
  DynamicArray<PostProcessPass*> passes;
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

/// Callbacks
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Renderer functions

void renderer_init(Window* window) {
  s_renderer.window = window;

  // GFX init

  sg_desc gfx_desc     = {};
  // @TODO: gfx_desc.logger.func = slog_func;

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

  // Pass init

  s_renderer.pass_action.colors[0].load_action = SG_LOADACTION_CLEAR;
  s_renderer.pass_action.colors[0].clear_value = sg_color{0.1f, 0.1f, 0.1f, 1.0f};
  
  // Listen to events
  
  event_register(EVENT_WINDOW_MAXIMIZED, window_resized_callback);
  event_register(EVENT_WINDOW_FRAMEBUFFER_RESIZED, window_resized_callback);
  event_register(EVENT_WINDOW_FULLSCREEN, window_resized_callback);

  // Done!
  FREYA_LOG_INFO("Successfully initialized the renderer context");
}

void renderer_shutdown() {
  // Destroy the framebuffers

  post_process_destroy(s_renderer.default_pass);
  for(PostProcessPass* pass : s_renderer.passes) {
    post_process_destroy(pass);
  }
  s_renderer.passes.clear();

  // GFX shutdown

  sgp_shutdown();
  sg_shutdown();

  // Done!
  FREYA_LOG_INFO("Successfully shutdown the renderer context");
}

void renderer_begin(Camera& camera) {
  FREYA_PROFILE_FUNCTION();

  IVec2 frame_size = window_get_framebuffer_size(s_renderer.window);
  IVec2 size       = window_get_size(s_renderer.window);

  // SGP begin

  sgp_begin(size.x, size.y);
  sgp_project(0.0f, (f32)camera.view_bounds.x, 0.0f, (f32)camera.view_bounds.y);

  // Pusing the camera transformation

  sgp_push_transform();

  sgp_translate(camera.position.x, camera.position.y);
  sgp_rotate(camera.rotation);
  sgp_scale(camera.zoom, camera.zoom);

  // Clear

  sgp_set_color(s_renderer.color.r, s_renderer.color.g, s_renderer.color.b, s_renderer.color.a);
  sgp_clear();

  // Setup the pass 

  s_renderer.pass.action  = s_renderer.pass_action;

  s_renderer.pass.swapchain.width  = frame_size.x;
  s_renderer.pass.swapchain.height = frame_size.y;

  s_renderer.pass.swapchain.sample_count   = window_get_samples_count(s_renderer.window);
  s_renderer.pass.swapchain.gl.framebuffer = 0; // @TODO (Renderer): Does this count all the time?

  // Begin the pass
  sg_begin_pass(&s_renderer.pass);
}

void renderer_end() {
  FREYA_PROFILE_FUNCTION();

  // Should be the camera's transform
  sgp_pop_transform();

  // End the painter 

  sgp_flush();
  sgp_end();

  // End the GFX

  sg_end_pass();
  sg_commit();
}

void renderer_set_clear_color(const Color& color) {
  s_renderer.color = color;
}

const Color& renderer_get_clear_color() {
  return s_renderer.color;
}

GfxContext* renderer_get_context() {
  return nullptr;
}

void renderer_push_post_process(PostProcessPass* pass) {
  FREYA_DEBUG_ASSERT(pass, "");

  // Push the pass to the back and set its previous

  if(s_renderer.passes.empty()) {
    pass->previous = s_renderer.default_pass;
  }
  else {
    pass->previous = s_renderer.passes.back();
  }

  s_renderer.passes.push_back(pass);

  // Some useful info
  FREYA_LOG_TRACE("Pushed post-process \'%s\' to the chain", pass->debug_name.c_str());
}

PostProcessPass* renderer_pop_post_process() {
  PostProcessPass* pass = s_renderer.passes.back();
  s_renderer.passes.pop_back();

  // Done!

  FREYA_LOG_TRACE("Popped post-process \'%s\' from the chain", pass->debug_name.c_str());
  return pass;
}

void renderer_queue_texture(GfxTexture* texture, 
                            const Rect2D& src, 
                            const Rect2D& dest, 
                            const f32 rotation,
                            const Color& tint) {
  // @TODO
}

void renderer_queue_texture(GfxTexture* texture, const Transform& transform, const Color& tint) {
  // @TODO
}

void renderer_queue_quad(const Transform& transform, const Color& color) {
  // @TODO
}

void renderer_queue_animation(const Animation& anim, const Transform& transform, const Color& tint) {
  // @TODO
}

void renderer_queue_particles(const ParticleEmitter& emitter) {
  // @TODO
}

void renderer_queue_debug_quad(const Transform& transform, const Color& color) {
  // @TODO
}

void renderer_queue_debug_polygon(const Transform& transform, const i32 sides, const Color& color) {
  // @TODO
}

void renderer_queue_debug_line(const Vec2& start, const Vec2& end, const Color& color) {
  // @TODO
}

/// Renderer functions
///---------------------------------------------------------------------------------------------------------------------

} // End of freya

//////////////////////////////////////////////////////////////////////////

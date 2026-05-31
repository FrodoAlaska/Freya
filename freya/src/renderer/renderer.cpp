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

  i32 textures_count = 0;
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

  // post_process_destroy(s_renderer.default_pass);
  for(PostProcessPass* pass : s_renderer.passes) {
    // post_process_destroy(pass);
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

  // Reset the renderer's state 

  s_renderer.textures_count = 0;

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

  // Reset the painter's state
  
  sgp_pop_transform();
  sgp_reset_blend_mode();

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

  sgp_draw_textured_rect(s_renderer.textures_count, 
                         {dest_pos.x, dest_pos.y, dest_size.x, dest_size.y},
                         {src_pos.x, src_pos.y, src_size.x, src_size.y}); 
}

void renderer_queue_texture(const Texture& texture, const Transform& transform, const Color& tint) {
  Rect2D src = {
    .size     = texture.size,
    .position = Vec2(0.0f),
  };
  
  Rect2D dest = {
    .size     = (Vec2)(texture.size) * transform.scale,
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
    if(emitter.texture.size.x != -1) {
      renderer_queue_texture(emitter.texture, emitter.transforms[i], emitter.color);
      continue;
    }

    renderer_queue_quad(emitter.transforms[i], emitter.color);
  }
}

/// Renderer functions
///---------------------------------------------------------------------------------------------------------------------

} // End of freya

//////////////////////////////////////////////////////////////////////////

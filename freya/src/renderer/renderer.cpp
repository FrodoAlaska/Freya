#include "freya_render.h"
#include "freya_logger.h"

//////////////////////////////////////////////////////////////////////////

namespace freya { // Start of freya

///---------------------------------------------------------------------------------------------------------------------
/// Consts

const sizei MAX_DRAW_CALLS = 2048;

/// Consts
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Renderer
struct Renderer {
  GfxContext* ctx         = nullptr;
  GfxContextDesc ctx_desc = {};

  Mat4 view        = Mat4(1.0f);
  Vec4 clear_color = Vec4(1.0f);
};

static Renderer s_renderer;
/// Renderer
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Private functions



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
  
  // Done!
  FREYA_LOG_INFO("Successfully initialized the renderer context");
}

void renderer_shutdown() {
  // Destroy the context
  gfx_context_shutdown(s_renderer.ctx);

  // Done!
  FREYA_LOG_INFO("Successfully shutdown the renderer context");
}

void renderer_begin(const Camera& camera) {
  // @TODO (Renderer)
 
  Vec4& col = s_renderer.clear_color;
  gfx_context_clear(s_renderer.ctx, col.r, col.g, col.b, col.a);
}

void renderer_end() {
  // @TODO (Renderer)
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
  // @TODO (Renderer)
}

void renderer_queue_texture(const AssetID& texture_id, const Vec2& position, const Vec2& size, const Vec4& tint) {
  // @TODO (Renderer)
}

void renderer_queue_quad(const Vec2& position, const Vec2& size, const Vec4& color) {
  // @TODO (Renderer)
}

/// Renderer functions
///---------------------------------------------------------------------------------------------------------------------

} // End of freya

//////////////////////////////////////////////////////////////////////////

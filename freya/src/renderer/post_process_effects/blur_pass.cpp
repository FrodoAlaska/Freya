#include "post_process_passes.h"
#include "../shaders/post_processing_shaders.h"

///---------------------------------------------------------------------------------------------------------------------
/// Blur pass functions

freya::PostProcessPass* blur_pass_create(freya::Window* window) {
  // Pass init
  
  freya::PostProcessPassDesc pass_desc;
  pass_desc.prepare_func = blur_pass_on_prepare;
  pass_desc.resize_func  = blur_pass_on_resize;
  
  pass_desc.frame_size  = freya::window_get_size(window);
  pass_desc.clear_color = freya::Color(1.0f);

  pass_desc.asset_group       = freya::ASSET_CACHE_ID;
  pass_desc.shader_context_id = asset_group_push_shader_context(pass_desc.asset_group, generate_blur_shader());

  pass_desc.clear_flags = (freya::GFX_CLEAR_FLAGS_COLOR_BUFFER);
  pass_desc.debug_name  = "Blur";

  pass_desc.attachments.emplace_back(freya::GFX_TEXTURE_FORMAT_RGBA32F);

  // Done!
  return freya::post_process_create(pass_desc);
}

void blur_pass_on_prepare(freya::PostProcessPass* pass) {
  // Using resources

  freya::GfxBindingDesc bind_desc = {
    .shader = pass->shader_context->shader, 
    
    .textures       = &pass->previous->outputs[0],
    .textures_count = 1,
  };
  freya::gfx_context_use_bindings(pass->gfx, bind_desc);

  // Setting outputs

  pass->outputs[0]    = pass->frame_desc.color_attachments[0];
  pass->outputs_count = 1;
}

void blur_pass_on_resize(freya::PostProcessPass* pass, const freya::IVec2& new_size) {
  pass->frame_size = new_size;

  // Updating the color attachment

  freya::GfxTextureDesc& color_desc = freya::gfx_texture_get_desc(pass->frame_desc.color_attachments[0]);

  color_desc.width  = (freya::u32)new_size.x;
  color_desc.height = (freya::u32)new_size.y;
  freya::gfx_texture_reload(pass->frame_desc.color_attachments[0], color_desc);

  // Update the framebuffer
  freya::gfx_framebuffer_update(pass->frame, pass->frame_desc);
}

/// Blur pass functions
///---------------------------------------------------------------------------------------------------------------------

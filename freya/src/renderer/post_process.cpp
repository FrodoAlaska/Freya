#include "freya_render.h"
#include "freya_logger.h"

#include "post_process_effects/post_process_passes.h"

//////////////////////////////////////////////////////////////////////////

namespace freya { // Start of freya

///---------------------------------------------------------------------------------------------------------------------
/// PostProcess functions

PostProcessPass* post_process_create(const PostProcessPassDesc& desc) {
  FREYA_ASSERT(desc.attachments.size() < RENDER_TARGETS_MAX, "Cannot add more than RENDER_TARGETS_MAX attachments");

  //
  // Pass init
  //

  PostProcessPass* pass = new PostProcessPass{};
  
  pass->gfx = renderer_get_context();

  pass->prepare_func = desc.prepare_func;
  pass->resize_func  = desc.resize_func;

  pass->frame_size  = desc.frame_size;
  pass->clear_color = desc.clear_color;
  pass->debug_name  = desc.debug_name;

  if(desc.shader_context_id != ASSET_ID_INVALID) {
    pass->shader_context = asset_group_get_shader_context(desc.shader_context_id);
  }

  //
  // Pass framebuffer init
  // 

  // Define the attachments of the pass
  // @TEMP

  GfxTextureDesc tex_desc = {
    .width  = (u32)pass->frame_size.x, 
    .height = (u32)pass->frame_size.y, 
    .filter = GFX_TEXTURE_FILTER_MIN_MAG_NEAREST,
  };

  for(auto& attachment : desc.attachments) {
    tex_desc.format = attachment;

    switch (attachment) {
      case GFX_TEXTURE_FORMAT_DEPTH16:
      case GFX_TEXTURE_FORMAT_DEPTH24:
      case GFX_TEXTURE_FORMAT_DEPTH32F:
      case GFX_TEXTURE_FORMAT_DEPTH_STENCIL_24_8:
        tex_desc.type = GFX_TEXTURE_DEPTH_TARGET;
        break;
      case GFX_TEXTURE_FORMAT_STENCIL8:
        tex_desc.type = GFX_TEXTURE_STENCIL_TARGET;
        break;
      default:
        tex_desc.type = GFX_TEXTURE_2D;
        break;
    }
    
    GfxTexture* texture = asset_group_get_texture(asset_group_push_texture(desc.asset_group, tex_desc));
    switch(tex_desc.type) {
      case GFX_TEXTURE_DEPTH_TARGET:
        pass->frame_desc.depth_attachment = texture;
        break;
      case GFX_TEXTURE_STENCIL_TARGET:
        pass->frame_desc.stencil_attachment = texture;
        break;
      default:
        pass->frame_desc.color_attachments[pass->frame_desc.attachments_count] = texture;
        pass->frame_desc.attachments_count++;
        break;
    }
  }
  
  pass->frame_desc.clear_flags = desc.clear_flags;
  pass->frame                  = gfx_framebuffer_create(pass->gfx, pass->frame_desc);

  // Done!
  return pass;
}

PostProcessPass* post_process_define_hdr(Window* window, Camera* camera) {
  return hdr_pass_create(window, camera); 
}

PostProcessPass* post_process_define_blur(Window* window) {
  return blur_pass_create(window);
}

PostProcessPass* post_process_define_greyscale(Window* window) {
  return greyscale_pass_create(window);
}

PostProcessPass* post_process_define_vignette(Window* window, const f32 intensity) {
  return vignette_pass_create(window, intensity);
}

void post_process_prepare(PostProcessPass* pass) {
  // Prepare the context

  gfx_context_set_target(pass->gfx, pass->frame);
  gfx_context_set_viewport(pass->gfx, 0, 0, pass->frame_size.x, pass->frame_size.y);

  Color& col = pass->clear_color;
  gfx_context_clear(pass->gfx, col.r, col.g, col.b, col.a);

  // Call the prepare function

  if(pass->prepare_func) {
    pass->prepare_func(pass);
  }
}

void post_process_destroy(PostProcessPass* pass) {
  if(!pass) {
    return;
  }

  gfx_framebuffer_destroy(pass->frame);
  delete pass;
}

/// PostProcess functions
///---------------------------------------------------------------------------------------------------------------------


} // End of freya

//////////////////////////////////////////////////////////////////////////

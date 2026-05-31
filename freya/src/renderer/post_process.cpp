#include "freya_render.h"
#include "freya_logger.h"

#include "post_process_effects/post_process_passes.h"

//////////////////////////////////////////////////////////////////////////

namespace freya { // Start of freya

///---------------------------------------------------------------------------------------------------------------------
/// PostProcess functions

PostProcessPass* post_process_create(Window* window, const PostProcessPassDesc& desc) {
  FREYA_ASSERT_LOG(desc.attachments.size() < RENDER_TARGETS_MAX, "Cannot add more than RENDER_TARGETS_MAX attachments");

  //
  // Post-process init
  //

  PostProcessPass* pass = new PostProcessPass{};

  pass->prepare_func = desc.prepare_func;
  pass->resize_func  = desc.resize_func;

  pass->frame_size  = desc.frame_size;
  pass->depth_clear = desc.depth_clear;

  pass->clear_color = desc.clear_color;
  pass->debug_name  = desc.debug_name;

  if(desc.shader_id != ASSET_ID_INVALID) {
    pass->shader = asset_group_get_shader(desc.shader_id);
  }

  //
  // Pass init
  //

  // Define the attachments of the pass

  i32 colors_count = 0;
  sg_pass& p       = pass->pass; 

  for(auto& attachment : desc.attachments) {
    switch(attachment) {
      case SG_PIXELFORMAT_DEPTH:
      case SG_PIXELFORMAT_DEPTH_STENCIL: {
        // Set up the image 

        sg_image_desc image_desc = {};
        
        image_desc.width  = pass->frame_size.x;
        image_desc.height = pass->frame_size.y;

        image_desc.pixel_format                   = attachment; 
        image_desc.usage.depth_stencil_attachment = true;

        // Set up the view
        
        sg_view_desc view_desc = {};

        view_desc.depth_stencil_attachment.image     = sg_make_image(image_desc);
        view_desc.depth_stencil_attachment.mip_level = 0;
        view_desc.depth_stencil_attachment.slice     = 0;

        // Create the view
        p.attachments.depth_stencil = sg_make_view(view_desc);

        // Set up the action 

        p.action.depth             = {};
        p.action.depth.clear_value = desc.depth_clear;
        
        p.action.stencil = {};
      } break;
      default: {
        // Set up the image 

        sg_image_desc image_desc = {};
        
        image_desc.width  = pass->frame_size.x;
        image_desc.height = pass->frame_size.y;

        image_desc.pixel_format           = attachment; 
        image_desc.usage.color_attachment = true;

        // Set up the view
        
        sg_view_desc view_desc = {};

        view_desc.color_attachment.image     = sg_make_image(image_desc);
        view_desc.color_attachment.mip_level = 0;
        view_desc.color_attachment.slice     = 0;

        // Create the view
        
        p.attachments.colors[colors_count] = sg_make_view(view_desc);

        // Set up the action 

        p.action.colors[colors_count]             = {};
        p.action.colors[colors_count].clear_value = {
          pass->clear_color.r, 
          pass->clear_color.g, 
          pass->clear_color.b, 
          pass->clear_color.a
        };

        // More colors!
        colors_count++;
      } break;
    }
  }

  // Apply the action 
  p.action = pass->action;
 
  // Setup the swapchain

  p.swapchain.width  = pass->frame_size.x;
  p.swapchain.height = pass->frame_size.y;

  p.swapchain.sample_count   = window_get_samples_count(window);
  p.swapchain.gl.framebuffer = renderer_get_post_process_count();

  // Done!
  return pass;
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
  // Call the prepare function

  if(pass->prepare_func) {
    pass->prepare_func(pass);
  }
}

void post_process_destroy(PostProcessPass* pass) {
  if(!pass) {
    return;
  }

  delete pass;
}

/// PostProcess functions
///---------------------------------------------------------------------------------------------------------------------


} // End of freya

//////////////////////////////////////////////////////////////////////////

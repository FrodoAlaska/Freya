#include "freya_render.h"
#include "freya_logger.h"

#include "post_process_effects/post_process_passes.h"

//////////////////////////////////////////////////////////////////////////

namespace freya { // Start of freya

///---------------------------------------------------------------------------------------------------------------------
/// PostProcess functions

PostProcessPass* post_process_allocate(Window* window) {
  // Allocate the pass

  PostProcessPass* pass = new PostProcessPass{};
  pass->window          = window;

  pass->attachments.reserve(RENDER_TARGETS_MAX);

  // Done!
  return pass;
}

void post_process_init(PostProcessPass* pass, const PostProcessPassDesc& desc) {
  FREYA_DEBUG_ASSERT(pass, "Invalid PostProcessPass object passed to `post_process_init");
  FREYA_DEBUG_ASSERT(desc.attachments.size() < RENDER_TARGETS_MAX, "Cannot add more than RENDER_TARGETS_MAX attachments");
  //
  // Post-process init
  //

  pass->prepare_func = desc.prepare_func;
  pass->resize_func  = desc.resize_func;

  pass->frame_size  = desc.frame_size;
  pass->depth_clear = desc.depth_clear;

  pass->clear_color = desc.clear_color;
  pass->debug_name  = desc.debug_name;

  //
  // Pass init
  //

  // Define the attachments of the pass

  i32 colors_count = 0;
  sg_pass& p       = pass->pass; 

  sg_pixel_format depth_format = SG_PIXELFORMAT_NONE;

  for(auto& attachment : desc.attachments) {
    switch(attachment) {
      case SG_PIXELFORMAT_DEPTH:
      case SG_PIXELFORMAT_DEPTH_STENCIL: {
        // Set up the image 

        sg_image_desc image_desc = {};
        
        image_desc.width        = pass->frame_size.x;
        image_desc.height       = pass->frame_size.y;
        image_desc.sample_count = desc.samples_count;

        image_desc.pixel_format                   = attachment; 
        image_desc.usage.depth_stencil_attachment = true;

        sg_image depth_img = sg_make_image(image_desc);

        // Set up the view
        
        sg_view_desc view_desc = {};

        view_desc.depth_stencil_attachment.image = depth_img;
        p.attachments.depth_stencil              = sg_make_view(view_desc);
        
        // Set up the action 

        p.action.depth             = {};
        p.action.depth.clear_value = desc.depth_clear;
        
        p.action.stencil = {};

        depth_format = attachment;
        
        // Create the texture view to use later

        view_desc.depth_stencil_attachment.image = {};
        view_desc.texture.image                  = depth_img;

        pass->attachments.emplace_back(sg_make_view(view_desc));
      } break;
      default: { // Color attachments
        // Set up the image 

        sg_image_desc image_desc = {};
        
        image_desc.width        = pass->frame_size.x;
        image_desc.height       = pass->frame_size.y;
        image_desc.sample_count = desc.samples_count;

        image_desc.pixel_format           = attachment; 
        image_desc.usage.color_attachment = true;

        sg_image color_img = sg_make_image(image_desc);

        // Set up the view
        
        sg_view_desc view_desc           = {};
        view_desc.color_attachment.image = color_img;

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

        // Create the texture view to use later

        view_desc.color_attachment.image = {};
        view_desc.texture.image          = color_img;
        
        pass->attachments.emplace_back(sg_make_view(view_desc));
      } break;
    }
  }

  // Apply the action 
  p.action = pass->action;

  // 
  // Pipeline init 
  //

  sg_pipeline_desc pipe_desc = {};

  pipe_desc.depth.pixel_format = depth_format;
  if(pipe_desc.depth.pixel_format != SG_PIXELFORMAT_NONE) {
    pipe_desc.depth.compare       = SG_COMPAREFUNC_LESS_EQUAL;
    pipe_desc.depth.write_enabled = true;
  }

  pipe_desc.shader       = asset_group_get_shader(desc.shader_id); 
  pipe_desc.sample_count = desc.samples_count;

  pipe_desc.layout.attrs[0].format = SG_VERTEXFORMAT_FLOAT2; // Position
  pipe_desc.layout.attrs[1].format = SG_VERTEXFORMAT_FLOAT2; // Texture coords

  pass->pipeline = sg_make_pipeline(pipe_desc);
}

PostProcessPass* post_process_create(Window* window, const PostProcessPassDesc& desc) {
  PostProcessPass* pass = post_process_allocate(window);
  post_process_init(pass, desc);

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

void post_process_destroy(PostProcessPass* pass) {
  pass->attachments.clear();
  delete pass;
}

/// PostProcess functions
///---------------------------------------------------------------------------------------------------------------------

} // End of freya

//////////////////////////////////////////////////////////////////////////

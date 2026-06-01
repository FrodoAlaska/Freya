#include "post_process_passes.h"
#include "../shaders/greyscale_shader.h"

///---------------------------------------------------------------------------------------------------------------------
/// Greyscale pass functions

freya::PostProcessPass* greyscale_pass_create(freya::Window* window) {
  // Pass init
  
  freya::PostProcessPassDesc pass_desc = {};
  pass_desc.resize_func                = greyscale_pass_on_resize; 

  pass_desc.frame_size = freya::window_get_size(window);
  pass_desc.group_id   = freya::ASSET_CACHE_ID;
  pass_desc.shader_id  = freya::asset_group_push_shader(pass_desc.group_id, *greyscale_shader_desc(sg_query_backend()));

  pass_desc.attachments.emplace_back(SG_PIXELFORMAT_RGBA8);

  pass_desc.debug_name    = "Greyscale";
  pass_desc.samples_count = freya::window_get_samples_count(window); 
 
  // Create the pass

  freya::PostProcessPass* pass = freya::post_process_create(window, pass_desc);

  pass->outputs[0]    = pass->attachments[0];
  pass->outputs_count = 1;

  // Done!
  return pass;
}

void greyscale_pass_on_resize(freya::PostProcessPass* pass, const freya::IVec2& new_size) {
  pass->frame_size = new_size;
  // @TODO
}

/// Greyscale pass functions
///---------------------------------------------------------------------------------------------------------------------

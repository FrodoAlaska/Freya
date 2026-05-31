#include "post_process_passes.h"

///---------------------------------------------------------------------------------------------------------------------
/// Greyscale pass functions

freya::PostProcessPass* greyscale_pass_create(freya::Window* window) {
  // Pass init
  
  freya::PostProcessPassDesc pass_desc;
  // @TODO

  // Done!
  return freya::post_process_create(window, pass_desc);
}

void greyscale_pass_on_prepare(freya::PostProcessPass* pass) {
  // @TODO
}

void greyscale_pass_on_resize(freya::PostProcessPass* pass, const freya::IVec2& new_size) {
  pass->frame_size = new_size;
  // @TODO
}

/// Greyscale pass functions
///---------------------------------------------------------------------------------------------------------------------

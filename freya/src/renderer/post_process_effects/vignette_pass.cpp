#include "post_process_passes.h"

///---------------------------------------------------------------------------------------------------------------------
/// PassState
struct PassState {
  freya::f32 intensity = 12.0f;
};

static PassState s_pass;
/// PassState
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Vignette pass functions

freya::PostProcessPass* vignette_pass_create(freya::Window* window, const freya::f32 intensity) {
  // @TODO
  freya::PostProcessPassDesc pass_desc;

  // Done!

  s_pass.intensity = intensity;
  return freya::post_process_create(window, pass_desc);
}

void vignette_pass_on_prepare(freya::PostProcessPass* pass) {
  // @TODO
}

void vignette_pass_on_resize(freya::PostProcessPass* pass, const freya::IVec2& new_size) {
  pass->frame_size = new_size;
  // @TODO
}

/// Vignette pass functions
///---------------------------------------------------------------------------------------------------------------------

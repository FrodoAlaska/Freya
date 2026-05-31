#pragma once

#include "freya_render.h"

///---------------------------------------------------------------------------------------------------------------------
/// Blur pass functions

freya::PostProcessPass* blur_pass_create(freya::Window* window);

void blur_pass_on_prepare(freya::PostProcessPass* pass);

void blur_pass_on_resize(freya::PostProcessPass* pass, const freya::IVec2& new_size);

/// Blur pass functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Greyscale pass functions

freya::PostProcessPass* greyscale_pass_create(freya::Window* window);

void greyscale_pass_on_prepare(freya::PostProcessPass* pass);

void greyscale_pass_on_resize(freya::PostProcessPass* pass, const freya::IVec2& new_size);

/// Greyscale pass functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Vignette pass functions

freya::PostProcessPass* vignette_pass_create(freya::Window* window, const freya::f32 intensity);

void vignette_pass_on_prepare(freya::PostProcessPass* pass);

void vignette_pass_on_resize(freya::PostProcessPass* pass, const freya::IVec2& new_size);

/// Vignette pass functions
///---------------------------------------------------------------------------------------------------------------------

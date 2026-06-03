#pragma once

#include "freya_render.h"

//////////////////////////////////////////////////////////////////////////

namespace freya { // Start of freya

///---------------------------------------------------------------------------------------------------------------------
/// UI renderer functions

bool ui_renderer_init(Window* window);

void ui_renderer_shutdown();

void ui_renderer_prepare();

bool ui_renderer_apply_font(const AssetID& font_id);

/// UI renderer functions
///---------------------------------------------------------------------------------------------------------------------

} // End of freya

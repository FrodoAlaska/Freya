#pragma once

#include <freya_app.h>

/// ----------------------------------------------------------------------
/// App functions 

bool app_init(const freya::Args& args, freya::Window* window);

void app_shutdown();

void app_update(freya::f32 dt);

void app_render();

void app_render_gui();

/// App functions 
/// ----------------------------------------------------------------------

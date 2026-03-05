#pragma once

#include <freya_app.h>

/// ----------------------------------------------------------------------
/// App functions 

freya::App* app_init(const freya::Args& args, freya::Window* window);

void app_shutdown(freya::App* app);

void app_update(freya::App* app, const freya::f32 delta_time);

void app_render(freya::App* app);

void app_render_gui(freya::App* app);

/// App functions 
/// ----------------------------------------------------------------------

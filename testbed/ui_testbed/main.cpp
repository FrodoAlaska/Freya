#include "app.h"

int main(int argc, char** argv) {
  freya::AppDesc app_desc {
    .init_fn     = app_init,
    .shutdown_fn = app_shutdown,
    .update_fn   = app_update, 
    .gui_fn      = app_render_gui, 

    .window_title  = "UI Testbed", 
    .window_width  = 1600, 
    .window_height = 900, 
    .window_flags  = (freya::i32)(freya::WINDOW_FLAGS_RESIZABLE | freya::WINDOW_FLAGS_CENTER_MOUSE),

    .args_values = argv, 
    .args_count  = argc,
  };
  return freya::engine_run(app_desc);
}

// FREYA_MAIN(engine_run);

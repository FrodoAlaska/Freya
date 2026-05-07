#include "app.h"

void engine_main() {
  freya::AppDesc app_desc {
    .init_fn     = app_init,
    .shutdown_fn = app_shutdown,
    .update_fn   = app_update, 
    
    .render_fn     = app_render, 
    .render_gui_fn = app_render_gui, 

    .window_title  = "Pathfinding Testbed", 
    .window_width  = 1600, 
    .window_height = 900, 
    .window_flags  = (freya::i32)(freya::WINDOW_FLAGS_RESIZABLE | freya::WINDOW_FLAGS_CENTER_MOUSE),
  };

  freya::engine_init(app_desc);
  freya::engine_run();
  freya::engine_shutdown();
}

FREYA_MAIN(engine_main);

#include "freya.h"

#if FREYA_PLATFORM_WEB == 1
  #include <emscripten/html5.h>
#endif

//////////////////////////////////////////////////////////////////////////

namespace freya { // Start of freya

/// ----------------------------------------------------------------------
/// Engine
struct Engine {
  AppDesc app_desc;
  Window* window;

  bool is_running;
};

static Engine s_engine;
/// Engine
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Macros

#define CHECK_VALID_CALLBACK(func, ...) if(func) func(__VA_ARGS__); 

/// Macros
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Callbacks

static bool on_app_quit(const Event& event, const void* dispatcher, const void* listener) {
  s_engine.is_running = false;
  window_set_should_close(s_engine.window, true);

  return false;
}

/// Callbacks
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Private functions

static void start_shutdown() {
  CHECK_VALID_CALLBACK(s_engine.app_desc.shutdown_fn);

  physics_world_shutdown();
  audio_device_shutdown();
  asset_manager_shutdown();
  
  renderer_shutdown();

  window_close(s_engine.window);
  event_shutdown();
}

static void update_and_render() {
  // Poll for input events
  window_poll_events(s_engine.window);

  // Physics world update
  physics_world_step();

  // Update
  CHECK_VALID_CALLBACK(s_engine.app_desc.update_fn, clock_get_delta_time());

  // Render 

  renderer_prepare();
  renderer_commit();

  // Render GUI
  CHECK_VALID_CALLBACK(s_engine.app_desc.gui_fn);

  // Update the internal systems

  input_update();
  clock_update();
}

static bool web_update_and_render(f64 dt, void* user_data) {
  // Start the shutdown process if we stop running

  if(!s_engine.is_running) {
    start_shutdown();
    return false;
  }
  
  // Main loop
  update_and_render();

  // Done!
  return s_engine.is_running;
}

static bool desktop_update_and_render() {
  // Main loop
  update_and_render();

  // Present
  window_swap_buffers(s_engine.window, s_engine.app_desc.has_vsync);

  // Done!
  
  s_engine.is_running = window_is_open(s_engine.window);
  return s_engine.is_running;
}

/// Private functions
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Engine functions

i32 engine_run(const AppDesc& desc) {
  //
  // Init 
  //

  // Engine init
  
  s_engine.app_desc   = desc; 
  s_engine.is_running = true;

  // Events init
  event_init();

  // Input init
  input_init();

  // Window init 
 
  WindowDesc window_desc = {
    .title         = desc.window_title, 
    .size          = IVec2(desc.window_width, desc.window_height),
    .flags         = desc.window_flags, 
    .samples_count = desc.samples_count,
  };

  s_engine.window = window_open(window_desc);
  FREYA_ASSERT(s_engine.window);

  // Useful input actions init
  
  InputAction action_desc = {
    .key_bind     = KEY_ENTER, 
    .mouse_bind   = MOUSE_BUTTON_LEFT, 
    .gamepad_bind = GAMEPAD_BUTTON_CROSS,
  };
  input_action_bind("ui-click", action_desc);
  
  action_desc = {
    .key_bind     = KEY_DOWN, 
    .gamepad_bind = GAMEPAD_BUTTON_DPAD_DOWN,
  };
  input_action_bind("ui-nav-down", action_desc);
  
  action_desc = {
    .key_bind     = KEY_UP, 
    .gamepad_bind = GAMEPAD_BUTTON_DPAD_UP,
  };
  input_action_bind("ui-nav-up", action_desc);
  
  // Asset manager init
  asset_manager_init();

  // Renderer init 
  renderer_init(s_engine.window);

  // Audio init
  audio_device_init(nullptr);

  // Physics world init
  physics_world_init(Vec2(0.0f, 9.81f));

  // Check for any command line arguments
  
  Args cli_args; 
  for(u32 i = 0; i < desc.args_count; i++) {
    cli_args.push_back(desc.args_values[i]);
  }

  // App init 
  
  FREYA_DEBUG_ASSERT(s_engine.app_desc.init_fn, "Cannot start the engine with an invalid application initialization callback");
  
  bool app_result = s_engine.app_desc.init_fn(cli_args, s_engine.window);
  if(!app_result) {
    FREYA_LOG_ERROR("Application \'%s\' failed to run", desc.window_title.c_str());
    return -1;
  }
 
  // Listen to events
  event_register(EVENT_APP_QUIT, on_app_quit);

  // Some useful info
  FREYA_LOG_INFO("Successfully initialized the application \'%s\'", desc.window_title.c_str());

  //
  // Loop
  //

#if FREYA_PLATFORM_WEB == 1
  emscripten_request_animation_frame_loop(web_update_and_render, nullptr);
#else
  while(desktop_update_and_render());
  start_shutdown(); // Called after the loop terminates
#endif

  // Done!
  
  FREYA_LOG_INFO("Appication \'%s\' was successfully shutdown", s_engine.app_desc.window_title.c_str());
  return 0;
}

/// Engine functions
/// ----------------------------------------------------------------------

} // End of freya

//////////////////////////////////////////////////////////////////////////

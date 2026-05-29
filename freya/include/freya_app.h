#pragma once

#include "freya_window.h"
#include "freya_containers.h"

#if FREYA_PLATFORM_WINDOWS == 1 
  ///
  /// @NOTE: 
  /// Since Windows likes to bloat the application 
  /// with unnecessary crap, this define should disable 
  /// that completely.
  ///

  #define WIN32_LEAN_AND_MEAN
  #include <windows.h>
#endif

//////////////////////////////////////////////////////////////////////////

namespace freya { // Start of freya

///---------------------------------------------------------------------------------------------------------------------
/// Macros 

#if FREYA_PLATFORM_WINDOWS == 1
 
  #define FREYA_MAIN(engine_main)                                                         \
  int WINAPI WinMain(HINSTANCE inst, HINSTANCE prev_inst, LPSTR cmd_line, int cmd_show) { \
    return engine_main(0, &cmd_line);                                                     \
  }                                                                                       \

#elif FREYA_PLATFORM_WEB == 1

  #define FREYA_MAIN(engine_main) \
  int main() {                    \
    engine_main(0, nullptr);      \
    return 0;                     \
  }                               \

#elif FREYA_PLATFORM_LINUX == 1

  #define FREYA_MAIN(engine_main)   \
  int main(int argc, char** argv) { \
    return engine_main(argc, argv); \
  }                                 \

#endif

/// Macros 
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Args 
using Args = DynamicArray<String>;  
/// Args 
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// App callbacks

/// A function callback called once upon the application's initialization, returning 
/// `true` on success, and `false` otherwise.
using AppInitFn       = std::function<bool(const Args& args, Window* window)>;

/// A function callback called once before the end of the application.
using AppShutdownFn   = std::function<void()>;

/// A function callback called every frame before rendering, passing 
/// in the delta time `dt` of the application.
using AppUpdateFn     = std::function<void(f32 dt)>;

/// A function callback called every frame after update, used for 
/// rendering purposes of the application.
using AppRenderPassFn = std::function<void()>;

/// App callbacks
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// App description
struct AppDesc {
  AppInitFn init_fn         = nullptr;
  AppShutdownFn shutdown_fn = nullptr;
  AppUpdateFn update_fn     = nullptr;
  
  AppRenderPassFn render_fn     = nullptr;
  AppRenderPassFn render_gui_fn = nullptr;
 
  String window_title;
  i32 window_width, window_height;

  i32 window_flags  = 0;
  i32 samples_count = 4;

  char** args_values = nullptr; 
  i32 args_count     = 0;
};
/// App description 
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Engine functions

/// Initiate the run loop of the engine, using the information in the given `desc` 
/// to initialize the application and all the engine's sub-systems.
///
/// This function will also run an infinite loop until the user requests to exit. 
/// The value returned from the function will determine the application's success or 
/// failure at running.
FREYA_API i32 engine_run(const AppDesc& desc);

/// Engine functions
///---------------------------------------------------------------------------------------------------------------------

} // End of freya

//////////////////////////////////////////////////////////////////////////

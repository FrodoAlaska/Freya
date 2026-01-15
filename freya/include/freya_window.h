#pragma once

#include "freya_math.h"

//////////////////////////////////////////////////////////////////////////

namespace freya { // Start of freya

///---------------------------------------------------------------------------------------------------------------------
/// Window
struct Window;
/// Window
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// WindowFlags
enum WindowFlags {
  /// No flags will be set when the window gets created
  WINDOW_FLAGS_NONE                = 1 << 0,

  /// Set the window to be resizable.
  WINDOW_FLAGS_RESIZABLE           = 1 << 1,

  /// The window will gain focus on creation.
  WINDOW_FLAGS_FOCUS_ON_CREATE     = 1 << 2, 

  /// The window will always gain focus when shown.
  WINDOW_FLAGS_FOCUS_ON_SHOW       = 1 << 3, 

  /// Minimize the window on creation.
  WINDOW_FLAGS_MINIMIZE            = 1 << 4, 
  
  /// Maximize the window on creation.
  WINDOW_FLAGS_MAXIMIZE             = 1 << 5, 
  
  /// Disable window decoration such as borders, widgets, etc. 
  /// Decorations will be enabled by default.
  WINDOW_FLAGS_DISABLE_DECORATIONS = 1 << 6,
  
  /// Center the mouse position relative to the screen on startup. 
  WINDOW_FLAGS_CENTER_MOUSE        = 1 << 7,
  
  /// Hide the cursor at creation. 
  /// The cursos will be shown by default.
  WINDOW_FLAGS_HIDE_CURSOR         = 1 << 8,

  /// Set the window to be fullscreen on creation. 
  WINDOW_FLAGS_FULLSCREEN          = 1 << 9,
};
/// WindowFlags
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Window functions

/// 
/// Creates a new window context and return the result. 
/// Returns a `nullptr` if the window was failed to be opened. 
///
/// `title`: The title of the new window. 
/// `width`: The width of the new window.
/// `height`: The height of the new window.
/// `flags`:
///   - `WINDOW_FLAGS_NONE`                = No flags will be set when the window gets created.
///   - `WINDOW_FLAGS_RESIZABLE`           = Set the window to be resizable.
///   - `WINDOW_FLAGS_FOCUS_ON_CREATE`     = The window will gain focus on creation.  
///   - `WINDOW_FLAGS_FOCUS_ON_SHOW`       = The window will always gain focus when shown. 
///   - `WINDOW_FLAGS_MINIMIZE`            = Minimize the window on creation. 
///   - `WINDOW_FLAGS_MAXIMIZE`            = Maximize the window on creation. 
///   - `WINDOW_FLAGS_DISABLE_DECORATIONS` = Disable window decoration such as borders, widgets, etc. Decorations will be enabled by default.
///   - `WINDOW_FLAGS_CENTER_MOUSE`        = Center the mouse position relative to the screen on startup.
///   - `WINDOW_FLAGS_HIDE_CURSOR`         = Hide the cursor at creation. The cursos will be shown by default.
///   - `WINDOW_FLAGS_FULLSCREEN`          = Set the window to be fullscreen on creation. 
/// 
FREYA_API Window* window_open(const char* title, const i32 width, const i32 height, i32 flags);

/// Closes the `window` context and clears up any memory.
FREYA_API void window_close(Window* window);

/// Poll events from the `window` context.
FREYA_API void window_poll_events(Window* window);

/// Swap the internal buffer of the `window` context every `interval` count. 
/// 
/// @NOTE: The `interval` parametar can be set as `0` if VSYNC is not needed.
/// Otherwise, `interval` can be set as `1` (which waits exactly `1` screen update) 
/// to disable VSYNC.
///
/// @NOTE: This might have no effect on some platforms.
FREYA_API void window_swap_buffers(Window* window, const i32 interval);

/// Returns `true` if the `window` context is still actively open. 
FREYA_API const bool window_is_open(const Window* window);

/// Returns `true` if the `window` context is currently in fullscreen mode.
FREYA_API const bool window_is_fullscreen(const Window* window);

/// Returns `true` if the `window` context is currently focused.
FREYA_API const bool window_is_focused(const Window* window);

/// Returns `true` if the `window` context is currently shown.
FREYA_API const bool window_is_shown(const Window* window);

/// Retrieve the current size of the `window` context.
FREYA_API const IVec2 window_get_size(const Window* window);

/// Retrieve the current framebuffer size of the `window` context.
FREYA_API const IVec2 window_get_framebuffer_size(const Window* window);

/// Retrieve the internal native handle of `window`.
FREYA_API void* window_get_handle(const Window* window);

/// Retrieve the current title of the `window` context.
FREYA_API const char* window_get_title(const Window* window);

/// Retrieve the current size of the monitor.
FREYA_API const IVec2 window_get_monitor_size(const Window* window);

/// Retrieve the aspect ratio of the `window` context.
FREYA_API const f32 window_get_aspect_ratio(const Window* window);

/// Retrieve the refresh rate of the monitor.
FREYA_API const f32 window_get_refresh_rate(const Window* window);

/// Retrieve the set window flags of `window`.
FREYA_API const WindowFlags& window_get_flags(const Window* window);

/// Retrieve the current position of the `window` context relative to the monitor.
FREYA_API const IVec2 window_get_position(const Window* window);

/// Set the given `window` as the current active context.
FREYA_API void window_set_current_context(Window* window);

/// Either disable or enable fullscreen mode on the `window` context.
FREYA_API void window_set_fullscreen(Window* window, const bool fullscreen);

/// Either show or hide the `window` context.
FREYA_API void window_set_show(Window* window, const bool show);

/// Set the size of the `window` to `size`.
FREYA_API void window_set_size(Window* window, const IVec2& size);

/// Set the title of the `window` to `title`.
FREYA_API void window_set_title(Window* window, const char* title);

/// Set the position of the `window` to `position`.
FREYA_API void window_set_position(Window* window, const IVec2& position);

/// Window functions
///---------------------------------------------------------------------------------------------------------------------

} // End of freya

//////////////////////////////////////////////////////////////////////////

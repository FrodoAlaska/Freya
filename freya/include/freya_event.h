#pragma once

#include "freya_base.h"

//////////////////////////////////////////////////////////////////////////

namespace freya { // Start of freya

///---------------------------------------------------------------------------------------------------------------------
/// EventType
enum EventType {
  EVENT_INVALID = -1,

  /// App events 
  
  EVENT_APP_QUIT = 0,

  /// Window events 
  
  EVENT_WINDOW_MOVED,
  EVENT_WINDOW_MINIMIZED, 
  EVENT_WINDOW_MAXIMIZED,
  EVENT_WINDOW_FOCUSED, 
  EVENT_WINDOW_RESIZED, 
  EVENT_WINDOW_FRAMEBUFFER_RESIZED, 
  EVENT_WINDOW_CLOSED, 
  EVENT_WINDOW_FULLSCREEN,

  /// Mouse events 
  
  EVENT_MOUSE_MOVED, 
  EVENT_MOUSE_BUTTON_PRESSED, 
  EVENT_MOUSE_BUTTON_RELEASED,
  EVENT_MOUSE_SCROLL_WHEEL,
  EVENT_MOUSE_CURSOR_SHOWN,
  EVENT_MOUSE_ENTER, 
  EVENT_MOUSE_LEAVE,

  /// Keyboard events 
  
  EVENT_KEY_PRESSED, 
  EVENT_KEY_RELEASED, 

  /// Joystick events
  
  EVENT_JOYSTICK_CONNECTED, 
  EVENT_JOYSTICK_DISCONNECTED, 

  EVENTS_MAX,
};
/// EventType
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Event
struct Event {
  /// The event's type.
  EventType type;
  
  /// New poisition of the window.
  i32 window_new_pos_x, window_new_pos_y;

  /// The current focus state of the window.
  bool window_has_focus;       

  /// The window's new size.
  i32 window_new_width, window_new_height;        

  /// The window's new size of the framebuffer.
  i32 window_framebuffer_width, window_framebuffer_height; 
 
  /// The window's new fullscreen state.
  bool window_is_fullscreen;
  
  /// The key that was just pressed.
  i32 key_pressed; 

  /// The key that was just released.
  i32 key_released;

  /// A key modifier that is passed on either 
  /// `EVENT_KEY_PRESSED`, `EVENT_KEY_RELEASED`, 
  /// `EVENT_MOUSE_BUTTON_PRESSED`, or `EVENT_MOUSE_BUTTON_RELEASED`.
  i32 key_modifier;
  
  /// The current mouse position (relative to the screen). 
  f32 mouse_pos_x, mouse_pos_y;    
  
  /// By how much did the mouse move since the last frame? 
  f32 mouse_offset_x, mouse_offset_y; 
   
  /// The mouse button that was just pressed.
  i32 mouse_button_pressed; 
 
  /// The mouse button that was just released.
  i32 mouse_button_released;
 
  /// The value the scroll mouse's wheel moved by. 
  f32 mouse_scroll_value; 

  /// Is the mouse cursor currently visible?
  bool cursor_shown;

  /// The joystick ID given to this event.
  i32 joystick_id; 
};
/// Event
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Event callback

/// The callback signature to use for any events, passing in the `event`, the `dispatcher`, and the `listener`.
using EventFireFn = std::function<bool(const Event& event, const void* dispatcher, const void* listener)>;

/// Event callback
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Event functions

/// Initialze the event system 
FREYA_API void event_init();

/// Shutdown the event system and reclaim some memory 
FREYA_API void event_shutdown();

/// Register a new listener to the event `type`, using the given `func`, and `listener` as the user data.
/// Later, when the function `event_dispatch` is called with the same `type`, `func` will be called, passing in `listener`.
FREYA_API void event_register(const EventType type, const EventFireFn& func, const void* listener = nullptr);

/// Initiate all callbacks associated with `event.type`, passing in `event` and `dispatcher`, returning `true` on success.
FREYA_API const bool event_dispatch(const Event& event, const void* dispatcher = nullptr);

/// Event functions
///---------------------------------------------------------------------------------------------------------------------

} // End of freya

//////////////////////////////////////////////////////////////////////////

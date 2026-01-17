#include "freya_window.h"
#include "freya_event.h"
#include "freya_input.h"
#include "freya_logger.h"
#include "freya_memory.h"

#include <GLFW/glfw3.h>

//////////////////////////////////////////////////////////////////////////

namespace freya { // Start of freya

///---------------------------------------------------------------------------------------------------------------------
/// Window
struct Window {
  GLFWwindow* handle  = nullptr;
  GLFWcursor* cursor  = nullptr;

  IVec2 size, old_size; 
  WindowFlags flags;

  f32 refresh_rate = 0.0f;

  bool is_fullscreen   = false;
  bool is_focused      = false;
  bool is_cursor_shown = true;

  IVec2 position, old_position; 

  Vec2 mouse_position      = Vec2(0.0f); 
  Vec2 last_mouse_position = Vec2(0.0f); 
  Vec2 mouse_offset        = Vec2(0.0f); 
};
/// Window
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Callbacks
static void error_callback(int err_code, const char* desc) {
  FREYA_LOG_FATAL("%s", desc);
}

static void window_pos_callback(GLFWwindow* handle, int xpos, int ypos) {
  Window* window     = (Window*)glfwGetWindowUserPointer(handle);
  window->position.x = xpos;
  window->position.y = ypos;
  
  event_dispatch(Event {
    .type = EVENT_WINDOW_MOVED, 

    .window_new_pos_x = xpos,
    .window_new_pos_y = ypos,
  });
}

static void window_maxmize_callback(GLFWwindow* handle, int maximized) {
  // Retrieving the size

  i32 width, height;
  glfwGetFramebufferSize(handle, &width, &height);
  
  Window* window = (Window*)glfwGetWindowUserPointer(handle);
  window->size.x = width;
  window->size.y = height;
 
  // Setting the event type accordingly

  EventType type; 
  if(maximized) {
    type = EVENT_WINDOW_MAXIMIZED;
  }
  else {
    type = EVENT_WINDOW_MINIMIZED;
  }

  // Dispatch an event

  event_dispatch(Event {
    .type = type,

    .window_framebuffer_width  = width,
    .window_framebuffer_height = height,
  });
}

static void window_focus_callback(GLFWwindow* handle, int focused) {
  Window* window     = (Window*)glfwGetWindowUserPointer(handle);
  window->is_focused = focused;

  event_dispatch(Event {
    .type = EVENT_WINDOW_FOCUSED, 

    .window_has_focus = window->is_focused,
  });
}

static void window_framebuffer_resize_callback(GLFWwindow* handle, int width, int height) {
  Window* window = (Window*)glfwGetWindowUserPointer(handle);
  window->size.x = width;
  window->size.y = height;
  
  event_dispatch(Event {
    .type = EVENT_WINDOW_FRAMEBUFFER_RESIZED, 

    .window_framebuffer_width  = width,
    .window_framebuffer_height = height,
  });
}

static void window_resize_callback(GLFWwindow* handle, int width, int height) {
  Window* window = (Window*)glfwGetWindowUserPointer(handle);
  window->size.x = width;
  window->size.y = height;
  
  event_dispatch(Event {
    .type = EVENT_WINDOW_RESIZED, 

    .window_new_width  = width,
    .window_new_height = height,
  });
}

static void window_close_callback(GLFWwindow* window) {
  event_dispatch(Event{.type = EVENT_WINDOW_CLOSED});
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
  if(action == GLFW_PRESS) {
    event_dispatch(Event {
      .type = EVENT_KEY_PRESSED, 

      .key_pressed  = key,
      .key_modifier = mods,
    });
  }
  else if(action == GLFW_RELEASE) {
    event_dispatch(Event {
      .type = EVENT_KEY_RELEASED, 
      
      .key_released = key,
      .key_modifier = mods,
    });
  }
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
  if(action == GLFW_PRESS) {
    event_dispatch(Event {
      .type = EVENT_MOUSE_BUTTON_PRESSED, 

      .key_modifier         = mods,
      .mouse_button_pressed = button,
    });
  }
  else if(action == GLFW_RELEASE) {
    event_dispatch(Event {
      .type = EVENT_MOUSE_BUTTON_RELEASED, 

      .key_modifier          = mods,
      .mouse_button_released = button,
    });
  }
}

void cursor_pos_callback(GLFWwindow* handle, double xpos, double ypos) {
  Window* window           = (Window*)glfwGetWindowUserPointer(handle);
  window->mouse_position.x = (f32)xpos;
  window->mouse_position.y = (f32)ypos;
 
  f32 offset_x = xpos - window->last_mouse_position.x; 
  f32 offset_y = window->last_mouse_position.y - ypos; 
  
  window->last_mouse_position.x = xpos; 
  window->last_mouse_position.y = ypos; 

  window->mouse_offset.x += offset_x;
  window->mouse_offset.y += offset_y;

  event_dispatch(Event {
    .type = EVENT_MOUSE_MOVED, 
    .mouse_pos_x = window->mouse_position.x, 
    .mouse_pos_y = window->mouse_position.y, 
    
    .mouse_offset_x = window->mouse_offset.x,
    .mouse_offset_y = window->mouse_offset.y,
  });
}

void cursor_enter_callback(GLFWwindow* window, int entered) {
  event_dispatch(Event {
    .type = entered == GLFW_TRUE ? EVENT_MOUSE_ENTER : EVENT_MOUSE_LEAVE,
  });
}

void scroll_wheel_callback(GLFWwindow* window, double xoffset, double yoffset) {
  event_dispatch(Event {
    .type               = EVENT_MOUSE_SCROLL_WHEEL, 
    .mouse_scroll_value = (f32)yoffset,
  });
}

void joystick_callback(int jid, int event) {
  EventType type = (event == GLFW_CONNECTED ? EVENT_JOYSTICK_CONNECTED : EVENT_JOYSTICK_DISCONNECTED); 

  event_dispatch(Event {
    .type        = type, 
    .joystick_id = jid,
  });
}

static bool freya_cursor_show_callback(const Event& event, const void* dispatcher, const void* listener) {
  Window* window = (Window*)listener;

  i32 cursor_mode = event.cursor_shown ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED;
  glfwSetInputMode(window->handle, GLFW_CURSOR, cursor_mode);

  return true;
}

static bool freya_quit_app_callback(const Event& event, const void* dispatcher, const void* listener) {
  Window* window = (Window*)listener;
  glfwSetWindowShouldClose(window->handle, GLFW_TRUE);

  return true;
}

/// Callbacks
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Private functions

static void set_window_hints(Window* window) {
  // Setting the this for the MSAA down the line
  // @TODO (Window): This should probably be configurable
  glfwWindowHint(GLFW_SAMPLES, 4); 
 
  // Setting the OpenGL context configurations
  
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // Error callback
  glfwSetErrorCallback(error_callback); 

  // Setting some defaults
  
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
  window->is_cursor_shown = true;

  // Setting the flags...

  if(IS_BIT_SET(window->flags, WINDOW_FLAGS_RESIZABLE)) {
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
  }
  
  if(IS_BIT_SET(window->flags, WINDOW_FLAGS_FOCUS_ON_CREATE)) {
    glfwWindowHint(GLFW_FOCUSED, GLFW_TRUE);
    window->is_focused = true;
  }
  
  if(IS_BIT_SET(window->flags, WINDOW_FLAGS_FOCUS_ON_SHOW)) {
    glfwWindowHint(GLFW_FOCUS_ON_SHOW, GLFW_TRUE);
    window->is_focused = true;
  }
  
  if(IS_BIT_SET(window->flags, WINDOW_FLAGS_MINIMIZE)) {
    glfwWindowHint(GLFW_MAXIMIZED, GLFW_FALSE);
  }
  
  if(IS_BIT_SET(window->flags, WINDOW_FLAGS_MAXIMIZE)) {
    glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);
  }
  
  if(IS_BIT_SET(window->flags, WINDOW_FLAGS_DISABLE_DECORATIONS)) {
    glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
  }
  
  if(IS_BIT_SET(window->flags, WINDOW_FLAGS_CENTER_MOUSE)) {
    glfwWindowHint(GLFW_CENTER_CURSOR, GLFW_TRUE);
  }
  
  if(IS_BIT_SET(window->flags, WINDOW_FLAGS_HIDE_CURSOR)) {
    window->is_cursor_shown = false;
  }
  
  if(IS_BIT_SET(window->flags, WINDOW_FLAGS_FULLSCREEN)) {
    window->is_fullscreen = true; 
  }
}

static void set_window_callbacks(Window* window) {
  // Window callbacks
  
  glfwSetWindowPosCallback(window->handle, window_pos_callback);
  glfwSetWindowMaximizeCallback(window->handle, window_maxmize_callback);
  glfwSetWindowFocusCallback(window->handle, window_focus_callback);
  glfwSetFramebufferSizeCallback(window->handle, window_framebuffer_resize_callback);
  glfwSetWindowSizeCallback(window->handle, window_resize_callback);
  glfwSetWindowCloseCallback(window->handle, window_close_callback);

  // Key callbacks 
  glfwSetKeyCallback(window->handle, key_callback);
  
  // Mouse callbacks 
  
  glfwSetMouseButtonCallback(window->handle, mouse_button_callback);
  glfwSetCursorPosCallback(window->handle, cursor_pos_callback);
  glfwSetCursorEnterCallback(window->handle, cursor_enter_callback);
  glfwSetScrollCallback(window->handle, scroll_wheel_callback);

  // Joystick callbacks 
  glfwSetJoystickCallback(joystick_callback); 

  // Listen to events
  
  event_register(EVENT_MOUSE_CURSOR_SHOWN, freya_cursor_show_callback, window);
  event_register(EVENT_APP_QUIT, freya_quit_app_callback, window);
}
/// Private functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Window functions

Window* window_open(const char* title, const i32 width, const i32 height, i32 flags) {
  Window* window = (Window*)memory_allocate(sizeof(Window));
  memory_zero(window, sizeof(Window));

  window->size     = IVec2(width, height);
  window->old_size = window->size;
  window->flags    = (WindowFlags)flags;

  // GLFW init and setup 
  
  glfwInit();
  set_window_hints(window);

  // This will make it easier to make the window fullscreen initially
  
  GLFWmonitor* monitor = nullptr;
  if(window->is_fullscreen) {
    monitor                = glfwGetPrimaryMonitor();
    const GLFWvidmode* vid = glfwGetVideoMode(monitor);

    // Reset the size of the window to the monitor's size
    window->size = IVec2(vid->width, vid->height);

    // Just in case the user decides to go back to 
    // non-fullscreen mode.
    window->old_position = IVec2(100);
  }

  // Creating the window
  window->handle = glfwCreateWindow(width, height, title, monitor, nullptr);

  // Setting the new refresh rate
  window->refresh_rate = glfwGetVideoMode(glfwGetPrimaryMonitor())->refreshRate;
  
  // Set some useful callbacks for later
  set_window_callbacks(window);
  
  // Something wrong...
  
  if(!window->handle) {
    memory_free(window);
    return nullptr;
  }

  // Setting our `window` as user data in the glfw window
  glfwSetWindowUserPointer(window->handle, window);
  
  // Querying data from the GLFW window
  
  glfwGetWindowPos(window->handle, &window->position.x, &window->position.y); 

  f64 mouse_pos_x, mouse_pos_y;
  glfwGetCursorPos(window->handle, &mouse_pos_x, &mouse_pos_y);

  window->mouse_position      = Vec2(mouse_pos_x, mouse_pos_y);
  window->last_mouse_position = window->mouse_position;
  
  // Setting the correct initial mouse offset
  window->mouse_offset = window->last_mouse_position - window->mouse_position;

  // Set the current context 
  glfwMakeContextCurrent(window->handle);

  // Set input mode
  
  i32 mode = (window->is_cursor_shown ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
  glfwSetInputMode(window->handle, GLFW_CURSOR, mode);
  
  FREYA_LOG_INFO("Window: {t = \"%s\", w = %i, h = %i} was successfully opened", title, width, height);
  return window;
}

void window_close(Window* window) {
  if(window->cursor) {
    glfwDestroyCursor(window->cursor);
  }

  glfwDestroyWindow(window->handle);
  glfwTerminate();
  
  memory_free(window);

  FREYA_LOG_INFO("Window was successfully closed");
}

void window_poll_events(Window* window) {
  glfwPollEvents();
}

void window_swap_buffers(Window* window, const i32 interval) {
  glfwSwapInterval(interval);
  glfwSwapBuffers(window->handle);
}

const bool window_is_open(const Window* window) {
  return !glfwWindowShouldClose(window->handle);
}

const bool window_is_fullscreen(const Window* window) {
  return window->is_fullscreen;
}

const bool window_is_focused(const Window* window) {
  return window->is_focused;
}

const bool window_is_shown(const Window* window) {
  return glfwGetWindowAttrib(window->handle, GLFW_VISIBLE);
}

const IVec2 window_get_size(const Window* window) {
  return window->size;
}

const IVec2 window_get_framebuffer_size(const Window* window) {
  i32 width, height;
  glfwGetFramebufferSize(window->handle, &width, &height);

  return IVec2(width, height);
}

void* window_get_handle(const Window* window) {
  return window->handle;
}

const char* window_get_title(const Window* window) {
  return glfwGetWindowTitle(window->handle);
}

const IVec2 window_get_monitor_size(const Window* window) {
  const GLFWvidmode* video_mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
  return IVec2(video_mode->width, video_mode->height);
}

const f32 window_get_aspect_ratio(const Window* window) {
  return ((f32)window->size.x / (f32)window->size.y);
}

const f32 window_get_refresh_rate(const Window* window) {
  return window->refresh_rate;
}

const WindowFlags& window_get_flags(const Window* window) {
  return window->flags;
}

const IVec2 window_get_position(const Window* window) {
  return window->position;
}

void window_set_current_context(Window* window) {
  glfwMakeContextCurrent(window->handle);
}

void window_set_fullscreen(Window* window, const bool fullscreen) {
  window->is_fullscreen         = fullscreen; 
  const GLFWvidmode* video_mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

  // Enabling or disabling fullscreen

  if(fullscreen) {
    window->old_size     = window->size;
    window->old_position = window->position;

    glfwSetWindowMonitor(window->handle, 
                         glfwGetPrimaryMonitor(), 
                         0, 0, 
                         video_mode->width, video_mode->height, 
                         window->refresh_rate);
  }
  else {
    glfwSetWindowMonitor(window->handle, 
                         nullptr, 
                         window->old_position.x, window->old_position.y, 
                         window->old_size.x, window->old_size.y, 
                         window->refresh_rate);
  }
  
  // Firing an event for the internal systems
 
  i32 framebuffer_width, framebuffer_height;
  glfwGetFramebufferSize(window->handle, &framebuffer_width, &framebuffer_height);

  Event event = {
    .type = EVENT_WINDOW_FULLSCREEN, 
    
    .window_new_width  = video_mode->width, 
    .window_new_height = video_mode->height,
    
    .window_framebuffer_width  = framebuffer_width,
    .window_framebuffer_height = framebuffer_height,

    .window_is_fullscreen = fullscreen,
  };
  event_dispatch(event, window);
}

void window_set_show(Window* window, const bool show) {
  if(show) {
    glfwShowWindow(window->handle);
  }
  else {
    glfwHideWindow(window->handle);
  }
}

void window_set_size(Window* window, const IVec2& size) {
  window->size = size; 
  glfwSetWindowSize(window->handle, size.x, size.y);
}

void window_set_title(Window* window, const char* title) {
  glfwSetWindowTitle(window->handle, title);
}

void window_set_position(Window* window, const IVec2& position) {
  window->position = position; 
  glfwSetWindowPos(window->handle, position.x, position.y);
}

/// Window functions
///---------------------------------------------------------------------------------------------------------------------

} // End of freya

//////////////////////////////////////////////////////////////////////////

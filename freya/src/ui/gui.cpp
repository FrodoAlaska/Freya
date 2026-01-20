#include "freya_ui.h"
#include "freya_logger.h"
#include "freya_memory.h"
#include "freya_input.h"
#include "freya_event.h"

#include <GLFW/glfw3.h>

#include <imgui/imgui.h>
#include <imgui/imgui_stdlib.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>
#include <imgui/backends/imgui_impl_opengl3_loader.h>

#include <glm/gtc/quaternion.hpp>

//////////////////////////////////////////////////////////////////////////

namespace freya { // Start of freya

///---------------------------------------------------------------------------------------------------------------------
/// GUIState
struct GUIState {
  Window* window          = nullptr; 
  GLFWwindow* glfw_window = nullptr;

  ImGuiIO io_config;
  GUIWindowFlags window_flags;

  f32 big_step   = 0.01f; 
  f32 small_step = 0.001f; 

  bool is_active = false;
};

static GUIState s_gui;
/// GUIState
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Callbacks

static bool on_keyboard_action(const Event& event, const void* dispatcher, const void* listener) {
  if(!s_gui.is_active) {
    return true;
  }

  switch(event.type) {
    case EVENT_KEY_PRESSED:
      if(event.key_pressed == KEY_LEFT_SHIFT) {
        s_gui.big_step   = 0.1f;
        s_gui.small_step = 0.01f;
      }
      break;
    case EVENT_KEY_RELEASED:
      if(event.key_released == KEY_LEFT_SHIFT) {
        s_gui.big_step   = 0.01f;
        s_gui.small_step = 0.001f;
      }
      break;
  }

  return true; 
}

static bool on_mouse_action(const Event& event, const void* dispatcher, const void* listener) {
  if(!s_gui.is_active) {
    return true;
  }

  switch(event.type) {
    case EVENT_MOUSE_BUTTON_PRESSED:
      input_cursor_show(!(event.mouse_button_pressed == MOUSE_BUTTON_LEFT));
      break;
    case EVENT_MOUSE_BUTTON_RELEASED:
      input_cursor_show((event.mouse_button_released == MOUSE_BUTTON_LEFT));
      break;
  }

  return true; 
}

/// Callbacks
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// GUI functions

bool gui_init(Window* window) {
  // Setting default values for the GUI
  
  s_gui        = GUIState{};
  s_gui.window = window;

  // Set up ImGui context
  
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();

  // Setting context flags
  
  io.ConfigFlags     = ImGuiConfigFlags_DockingEnable;
  s_gui.window_flags = GUI_WINDOW_FLAGS_NONE;

  // Dark mode WOOOOOOOAH! 
  ImGui::StyleColorsDark();

  // Setting up the glfw backend
 
  s_gui.glfw_window = (GLFWwindow*)window_get_handle(window);

  if(!ImGui_ImplGlfw_InitForOpenGL(s_gui.glfw_window, true)) {
    FREYA_LOG_ERROR("Failed to initialize GLFW for ImGui");
    return false;
  }
  
  // Setting up the opengl backend
  
  if(!ImGui_ImplOpenGL3_Init("#version 460 core")) {
    FREYA_LOG_ERROR("Failed to initialize OpenGL for ImGui");
    return false;
  }

  // Listen to events
  
  event_register(EVENT_KEY_PRESSED, on_keyboard_action);
  event_register(EVENT_KEY_RELEASED, on_keyboard_action);
  
  event_register(EVENT_MOUSE_BUTTON_PRESSED, on_mouse_action);
  event_register(EVENT_MOUSE_BUTTON_RELEASED, on_mouse_action);

  return true;
}

void gui_shutdown() {
  ImGui_ImplGlfw_Shutdown();
  ImGui_ImplOpenGL3_Shutdown();
  ImGui::DestroyContext();
}

void gui_begin() {
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
}

void gui_end() {
  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void gui_set_window_flags(const i32 flags) {
  s_gui.window_flags = (GUIWindowFlags)flags;
}

bool gui_begin_panel(const char* name) {
  return ImGui::Begin(name, nullptr, (ImGuiWindowFlags)s_gui.window_flags);
}

void gui_end_panel() {
  ImGui::End();
}

void gui_toggle_active() {
  s_gui.is_active = !s_gui.is_active;
}

const bool gui_is_focused() {
  return ImGui::GetIO().WantCaptureMouse;
}

const bool gui_is_active() {
  return s_gui.is_active;
}

void gui_renderer_info() {
  if(!gui_begin_panel("Renderer Info")) {
    gui_end_panel();
    return;
  }
 
  // Clear color
 
  Vec4 clear_color = renderer_get_clear_color();
  bool is_picked   = ImGui::ColorPicker4("Clear color", &clear_color[0], ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoSmallPreview);

  if(is_picked) {
    renderer_set_clear_color(clear_color);
  }

  gui_end_panel();
}

void gui_window_info() {
  if(!gui_begin_panel("Window")) {
    gui_end_panel();
    return;
  }
 
  // Title
 
  String window_title = window_get_title(s_gui.window);
  if(ImGui::InputText("Title", &window_title)) {
    window_set_title(s_gui.window, window_title.c_str());
  }
 
  // Size 
  
  IVec2 window_size = window_get_size(s_gui.window);
  if(ImGui::SliderInt2("Size", &window_size[0], 0, 1920)) {
    window_set_size(s_gui.window, window_size);
  }

  // Position 
  
  IVec2 window_pos = window_get_position(s_gui.window);
  if(ImGui::SliderInt2("Position", &window_pos[0], 0, 1920)) {
    window_set_position(s_gui.window, window_pos);
  }

  // Fullscreen 

  bool is_fullscreen = window_is_fullscreen(s_gui.window);
  if(ImGui::Checkbox("Fullscreen", &is_fullscreen)) {
    window_set_fullscreen(s_gui.window, is_fullscreen);
  }

  gui_end_panel();
}

void gui_debug_info() {
  if(!gui_begin_panel("Debug Info")) {
    gui_end_panel();
    return;
  }

  // FPS 
  
  ImGui::Text("FPS: %.3lf", clock_get_fps());
  ImGui::Separator();

  // Mouse
 
  if(ImGui::CollapsingHeader("Mouse")) {
    Vec2 mouse_pos    = input_mouse_position();
    Vec2 mouse_offset = input_mouse_offset();

    ImGui::Text("Position: %s", vec2_to_string(mouse_pos).c_str());
    ImGui::Text("Offset: %s", vec2_to_string(mouse_offset).c_str());
  } 

  // Gamepad

  if(ImGui::CollapsingHeader("Gamepads")) {
    for(sizei i = JOYSTICK_ID_0; i < JOYSTICK_ID_LAST; i++) {
      JoystickID joy_id = (JoystickID)i;

      if(!input_gamepad_connected(joy_id)) {
        continue;
      }

      // Name 
      
      String name = ("Gamepad" + String(input_gamepad_get_name(joy_id)));
      ImGui::SeparatorText(name.c_str());

      // Axises values
    
      Vec2 left_axis = input_gamepad_axis_value(joy_id, GAMEPAD_AXIS_LEFT); 
      ImGui::Text("Left axis: X = %0.3f, Y = %0.3f", left_axis.x, left_axis.y);
   
      Vec2 right_axis = input_gamepad_axis_value(joy_id, GAMEPAD_AXIS_RIGHT); 
      ImGui::Text("Right axis: X = %0.3f, Y = %0.3f", right_axis.x, right_axis.y);
   
      Vec2 triggers = input_gamepad_axis_value(joy_id, GAMEPAD_AXIS_TRIGGER); 
      ImGui::Text("Trigger: Left = %0.3f, Right = %0.3f", triggers.x, triggers.y);
    }
  }

  // Memory
 
  if(ImGui::CollapsingHeader("Memory")) {
    ImGui::Text("Allocations: %zu", memory_get_allocations_count());

    sizei mbytes = memory_get_allocation_bytes() / MiB(1);
    ImGui::Text("Bytes allocated: %zuMiB", mbytes);
  } 

  gui_end_panel();
}

void gui_edit_color(const char* name, Vec4& color) {
  ImGui::SeparatorText(name); 
  ImGui::PushID(name); 

  ImGui::ColorEdit4(name, &color[0]);

  ImGui::PopID();
}

void gui_edit_transform(const char* name, Transform* transform) {
  ImGui::SeparatorText(name); 
  ImGui::PushID(name); 

  //
  // SRT translation
  // 
  
  // Position
  
  if(ImGui::DragFloat2("Position", &transform->position[0], s_gui.big_step)) {
    transform_translate(*transform, transform->position);
  }

  // Scale

  if(ImGui::DragFloat2("Scale", &transform->scale[0], s_gui.big_step)) {
    transform_scale(*transform, transform->scale);
  }

  // Rotation

  if(ImGui::DragFloat("Rotation (in radians)", &transform->rotation, s_gui.big_step)) {
    transform_rotate(*transform, transform->rotation);
  }
  
  ImGui::PopID(); 
}

void gui_edit_camera(const char* name, Camera* camera) {
  ImGui::SeparatorText(name); 
  ImGui::PushID(name); 

  //
  // Transfrom
  //

  // Position
  
  if(ImGui::DragFloat2("Position", &camera->transform.position[0], s_gui.big_step)) {
    transform_translate(camera->transform, camera->transform.position);
  }

  // Zoom

  if(ImGui::DragFloat("Zoom", &camera->transform.scale[0], s_gui.big_step)) {
    transform_scale(camera->transform, camera->transform.scale);
  }

  // Rotation

  if(ImGui::DragFloat("Rotation (in radians)", &camera->transform.rotation, s_gui.big_step)) {
    transform_rotate(camera->transform, camera->transform.rotation);
  }

  // Sensitivity
  ImGui::DragFloat("Sensitivity", &camera->sensitivity, s_gui.small_step);

  // Exposure
  ImGui::DragFloat("Exposure", &camera->exposure, s_gui.small_step);

  ImGui::PopID(); 
}

void gui_edit_audio_source(const char* name, AudioSourceID& source) {
  ImGui::SeparatorText(name); 
  ImGui::PushID(name); 

  AudioSourceDesc source_desc = audio_source_get_desc(source);

  //
  // Source Info
  // 

  // Volume*
  
  if(ImGui::SliderFloat("Volume", &source_desc.volume, 0.0f, 1.0f)) {
    audio_source_set_volume(source, source_desc.volume);
  }

  // Pitch
  
  if(ImGui::SliderFloat("Pitch", &source_desc.pitch, 0.0f, 1.0f)) {
    audio_source_set_pitch(source, source_desc.pitch);
  }

  // Position
  
  if(ImGui::DragFloat2("Position", &source_desc.position[0], s_gui.small_step, -100.0f, 100.0f)) {
    audio_source_set_position(source, source_desc.position);
  }

  // Velocity
  
  if(ImGui::DragFloat2("Velocity", &source_desc.velocity[0], s_gui.small_step, -1.0f, 1.0f)) {
    audio_source_set_velocity(source, source_desc.velocity);
  }
  
  // Direction
  
  if(ImGui::DragFloat2("Direction", &source_desc.direction[0], s_gui.small_step, -1.0f, 1.0f)) {
    audio_source_set_direction(source, source_desc.direction);
  }
  
  // Looping
  
  if(ImGui::Checkbox("Looping", &source_desc.is_looping)) {
    audio_source_set_looping(source, source_desc.is_looping);
  }

  // Queued buffers 
  ImGui::Text("Current queued buffers %zu", source_desc.buffers_count);

  //
  // Command buttons
  // 

  // Play button
  
  if(ImGui::Button("Play")) {
    audio_source_start(source);
  } 

  ImGui::SameLine();
  
  // Pause button
  
  if(ImGui::Button("Pause")) {
    audio_source_pause(source);
  } 
  
  ImGui::SameLine();
  
  // Stop button
  
  if(ImGui::Button("Stop")) {
    audio_source_stop(source);
  } 
  
  ImGui::SameLine();

  // Restart button
  
  if(ImGui::Button("Restart")) {
    audio_source_restart(source);
    audio_source_start(source);
  } 
  
  ImGui::SameLine();

  ImGui::NewLine();
  ImGui::PopID(); 
}

void gui_edit_audio_listener(const char* name) {
  ImGui::SeparatorText(name); 
  ImGui::PushID(name); 

  AudioListenerDesc listener = audio_listener_get_desc();

  // Volume
  
  if(ImGui::SliderFloat("Volume", &listener.volume, 0.0f, 1.0f)) {
    audio_listener_set_volume(listener.volume);
  }

  // Position
  
  if(ImGui::DragFloat2("Position", &listener.position[0], s_gui.small_step, -1.0f, 1.0f)) {
    audio_listener_set_position(listener.position);
  }

  // Velocity
  
  if(ImGui::DragFloat2("Velocity", &listener.velocity[0], s_gui.small_step, -1.0f, 1.0f)) {
    audio_listener_set_velocity(listener.velocity);
  }

  ImGui::PopID(); 
}

void gui_edit_timer(const char* name, Timer* timer) {
  ImGui::SeparatorText(name); 
  ImGui::PushID(name); 

  // Limit 
  {
    ImGui::DragFloat("Limit", &timer->limit, s_gui.big_step, 0.0f);
  }

  // Booleans
  {
    ImGui::Checkbox("One shot", &timer->is_one_shot);
    ImGui::Checkbox("Active", &timer->is_active);
  }
  
  ImGui::PopID(); 
}

void gui_edit_animation(const char* name, Animation* anim) {
  ImGui::SeparatorText(name); 
  ImGui::PushID(name); 
  
  // Current frame
  ImGui::DragInt("Current frame", &anim->current_frame, s_gui.small_step, 0, anim->frames_count);
 
  // Direction
  ImGui::DragInt("Direction", &anim->direction, s_gui.small_step, -1, 1);
 
  // Speed
  ImGui::DragFloat("Speed", &anim->flip_speed, s_gui.small_step, 0.0f);

  // Booleans
  
  ImGui::Checkbox("Active", &anim->is_active);
  ImGui::Checkbox("Loop", &anim->can_loop);
  ImGui::Checkbox("Alternate", &anim->can_alternate);
  
  ImGui::PopID(); 
}

void gui_edit_particle_emitter(const char* name, ParticleEmitter* emitter) {
  ImGui::SeparatorText(name); 
  ImGui::PushID(name); 

  //
  // Physics
  //

  ImGui::DragFloat2("Position", &emitter->initial_position[0], s_gui.big_step);
  ImGui::DragFloat2("Velocity", &emitter->initial_velocity[0], s_gui.big_step);
  
  ImGui::DragFloat("Lifetime", &emitter->lifetime.limit, s_gui.big_step, 0.0f, 512.0f);
  ImGui::DragFloat("Gravity", &emitter->gravity_factor, s_gui.big_step);
 
  // Rendering
  
  ImGui::DragFloat2("Scale", &emitter->initial_scale[0], s_gui.big_step);
  ImGui::ColorEdit4("Color", &emitter->color[0]);

  //
  // Distribution
  //
  
  ImGui::DragFloat("Distribution radius", &emitter->distribution_radius, s_gui.big_step);

  i32 current_dist = emitter->distribution;
  if(ImGui::Combo("Distributions", &current_dist, "Random\0Square\0Circular\0\0")) {
    emitter->distribution = (ParticleDistributionType)current_dist;
  }

  //
  // Particles count
  // 

  i32 count = (i32)emitter->particles_count;
  if(ImGui::SliderInt("Count", &count, 1, (i32)(PARTICLES_MAX - 1))) {
    emitter->particles_count = (sizei)count;
  }
  
  ImGui::PopID(); 
}

/// Editor functions
///---------------------------------------------------------------------------------------------------------------------

} // End of freya

//////////////////////////////////////////////////////////////////////////

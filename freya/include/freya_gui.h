#pragma once

#include "freya_entity.h"

//////////////////////////////////////////////////////////////////////////

namespace freya { // Start of freya

///---------------------------------------------------------------------------------------------------------------------
/// GUIWindowFlags
enum GUIWindowFlags {
  /// No flags set.
  GUI_WINDOW_FLAGS_NONE                         = 0, 
  
  /// Windows will not have a title bar.
  GUI_WINDOW_FLAGS_NO_TITLE_BAR                 = 1 << 0, 
  
  /// Windows will not resize.
  GUI_WINDOW_FLAGS_NO_RESIZE                    = 1 << 1, 
  
  /// Windows will not move.
  GUI_WINDOW_FLAGS_NO_MOVE                      = 1 << 2, 
  
  /// Windows will not have a scroll bar.
  GUI_WINDOW_FLAGS_NO_SCROLL_BAR                = 1 << 3, 
  
  /// Windows will not scroll with the mouse.
  GUI_WINDOW_FLAGS_NO_SCROLL_WITH_MOUSE         = 1 << 4, 
  
  /// Windows will not collapse.
  GUI_WINDOW_FLAGS_NO_COLLAPSE                  = 1 << 5, 
  
  /// Windows will always resize on content update.
  GUI_WINDOW_FLAGS_AUTO_RESIZE                  = 1 << 6, 
  
  /// Windows will not have a background.
  GUI_WINDOW_FLAGS_NO_BACKGROUND                = 1 << 7, 
  
  /// Windows will not load settings from an `.ini` file.
  GUI_WINDOW_FLAGS_NO_SAVED_SETTINGS            = 1 << 8, 
  
  /// Windows will not catch mouse inputs.
  GUI_WINDOW_FLAGS_NO_MOUSE_INPUTS              = 1 << 9, 
  
  /// Windows will have a menu bar.
  GUI_WINDOW_FLAGS_MENU_BAR                     = 1 << 10, 
  
  /// Windows will have a horizontal scroll bar.
  GUI_WINDOW_FLAGS_HORIZONTAL_SCROLL_BAR        = 1 << 11, 
  
  /// Windows will have no focus once created.
  GUI_WINDOW_FLAGS_NO_FOCUS_ON_APPEAR           = 1 << 12, 
  
  /// Windows will not be brought to the front on focus.
  GUI_WINDOW_FLAGS_NO_BRING_TO_FRONT            = 1 << 13, 
  
  /// Windows will always have vertical scroll bars.
  GUI_WINDOW_FLAGS_ALWAYS_VERTICAL_SCROLL_BAR   = 1 << 14, 
  
  /// Windows will always have horizontal scroll bars.
  GUI_WINDOW_FLAGS_ALWAYS_HORIZONTAL_SCROLL_BAR = 1 << 15, 
  
  /// Windows will not catch any navigation inputs.
  GUI_WINDOW_FLAGS_NO_NAV_INPUTS                = 1 << 16, 
  
  /// Windows will not be focused on any navigation inputs.
  GUI_WINDOW_FLAGS_NO_NAV_FOCUS                 = 1 << 17, 
  
  /// Windows will have a little dot indicating that they were changed.
  GUI_WINDOW_FLAGS_NO_UNSAVED_DOCUMENT          = 1 << 18, 
  
  /// Windows cannot be docked.
  GUI_WINDOW_FLAGS_NO_DOCKING                   = 1 << 19, 
};
/// GUIWindowFlags
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// GUI functions

/// Initialize the GUI context, given the information in `window`.
FREYA_API bool gui_init(Window* window);

/// Shutdown the GUI context, reclaiming any allocated memory.
FREYA_API void gui_shutdown();

/// Begin a frame of the GUI context. 
///
/// @NOTE: This MUST be called BEFORE any GUI context calls.
FREYA_API void gui_begin();

/// End a frame of the GUI context. 
///
/// @NOTE: This MUST be called BEFORE the end of the GUI frame.
FREYA_API void gui_end();

/// Set the window flags to `flags` of the current GUI context.
///
/// @NOTE: Check `GUIWindowFlags` for more information about specific flags.
/// Also, this function _must_ be called before any calls to `gui_begin_panel` 
/// to see the effects.
FREYA_API void gui_set_window_flags(const i32 flags);

/// Start a panel with the name `name`.
FREYA_API bool gui_begin_panel(const char* name);

/// End the panel with the latest `gui_begin_panel` call.
FREYA_API void gui_end_panel();

/// Toggle between an inactive and active GUI. 
FREYA_API void gui_toggle_active();

/// Returns `true` if the GUI is currently active. 
FREYA_API const bool gui_is_active();

/// Returns `true` if the GUI is currently focused or capturing the mouse. 
FREYA_API const bool gui_is_focused();

/// Draw a preset panel with all of the renderer information.
FREYA_API void gui_renderer_info();

/// Draw a preset panel with all of the window information.
FREYA_API void gui_window_info();

/// Draw a preset panel with all of the debug information.
FREYA_API void gui_debug_info();

/// Add a color wheel identified by `name` to edit the given `color`.
FREYA_API void gui_edit_color(const char* name, Vec4& color);

/// Add a transform section identified by `name` to edit the given `transform`.
FREYA_API void gui_edit_transform(const char* name, Transform* transform);

/// Add a camera section identified by `name` to edit the given `camera`.
FREYA_API void gui_edit_camera(const char* name, Camera* camera);

/// Add an audio source section identified by `name` to edit the given `source`.
FREYA_API void gui_edit_audio_source(const char* name, AudioSourceID& source);

/// Add an audio listener section identified by `name` to edit the global audio listener.
FREYA_API void gui_edit_audio_listener(const char* name);

/// Add a timer section identified by `name` to edit the given `timer`.
FREYA_API void gui_edit_timer(const char* name, Timer* timer);

/// Add an animation section identified by `name` to edit the given `anim`.
FREYA_API void gui_edit_animation(const char* name, Animation* anim);

/// Add an animator section identified by `name` to edit the given `anim`.
FREYA_API void gui_edit_animator(const char* name, Animator* anim);

/// Add a particle emitter section identified by `name` to edit the given `emitter`.
FREYA_API void gui_edit_particle_emitter(const char* name, ParticleEmitter* emitter);

/// Add a physics body section identified by `name` to edit the given `body`.
FREYA_API void gui_edit_physics_body(const char* name, PhysicsBodyID& body);

/// Add a noise generator section identified by `name` to edit the given `gen`.
FREYA_API void gui_edit_noise_generator(const char* name, NoiseGenerator* gen);

/// Add a sprite component section identified by `name` to edit the given `sprite`.
FREYA_API void gui_edit_sprite_component(const char* name, SpriteComponent* sprite);

/// Add a tile sprite component section identified by `name` to edit the given `sprite`.
FREYA_API void gui_edit_tile_sprite_component(const char* name, TileSpriteComponent* sprite);

/// Add a poisson disk section identified by `name` to edit the given `disk`.
FREYA_API void gui_edit_poisson_disk(const char* name, PoissonDiskDesc* disk);

/// Add an entity section identified by `name` to edit the given `entt`.
FREYA_API void gui_edit_entity(const char* name, EntityWorld& world, Entity& entt);

/// GUI functions
///---------------------------------------------------------------------------------------------------------------------

} // End of freya

//////////////////////////////////////////////////////////////////////////

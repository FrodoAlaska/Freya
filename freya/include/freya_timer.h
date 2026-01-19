#pragma once

#include "freya_base.h"
#include "freya_containers.h"

#include <tracy/Tracy.hpp>

//////////////////////////////////////////////////////////////////////////

namespace freya { // Start of freya

///---------------------------------------------------------------------------------------------------------------------
/// PerfTimer 
struct PerfTimer {
  std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
  
  f32 to_milliseconds = 0.0f;
  f32 to_seconds      = 0.0f;
  f32 to_minutes      = 0.0f;
};
/// PerfTimer 
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// PerfTimer functions

/// Start the performance timer `timer`.
FREYA_API void perf_timer_start(PerfTimer& timer);

/// Stop the performance timer `timer`.
FREYA_API void perf_timer_stop(PerfTimer& timer);

/// Convert the contents of `timer` to a string format.
FREYA_API String perf_timer_to_string(PerfTimer& timer);

/// PerfTimer functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Macros

#if FREYA_BUILD_DEBUG == 1 
  #define FREYA_PERF_TIMER_BEGIN(timer)    freya::perf_timer_start(timer);
  #define FREYA_PERF_TIMER_END(timer, tag) freya::perf_timer_stop(timer); FREYA_LOG_TRACE("\'%s\' took %s to run", tag, freya::perf_timer_to_string(timer).c_str());
#else
  #define FREYA_PERF_TIMER_BEGIN(timer)
  #define FREYA_PERF_TIMER_END(timer, tag)
#endif

#define FREYA_PROFILE_FUNCTION()           ZoneScoped
#define FREYA_PROFILE_FUNCTION_NAMED(name) ZoneScopedN(name)

/// Macros
///---------------------------------------------------------------------------------------------------------------------


///---------------------------------------------------------------------------------------------------------------------
/// Timer 
struct Timer {
  /// The internal counter of the timer.
  f32 counter = 0.0f; 

  /// The maximum limit of the timer.
  f32 limit   = 0.0f;

  /// Determines if the timer is one shot or not.
  bool is_one_shot = false;

  /// Determines if the timer is currently active or not.
  bool is_active   = true;

  /// Determines if the tiemr has run out or still going.
  bool has_runout  = false;
};
/// Timer 
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Timer functions

/// Write the values to `out_timer` with the given information.
FREYA_API void timer_create(Timer& out_timer, const f32 limit, const bool one_shot, const bool initial_active = true);

/// Let the given `timer` tick, scaled by `delta_time.
FREYA_API void timer_update(Timer& timer, const f32 delta_time);

/// Reset the `timer` to its initial state.
FREYA_API void timer_reset(Timer& timer);

/// Timer functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Clock functions

/// Updates the values of the time, FPS (frames per second), and the delta time.
/// 
/// @NOTE: This must be called _every_ frame.
FREYA_API void clock_update();

/// Retrieve the time passed since the application was initialzed.
FREYA_API const f64 clock_get_time(); 

/// Retrieve the current FPS (frames per second) of the application
FREYA_API const f64 clock_get_fps();

/// Retrieve the time passed between each frame. 
FREYA_API const f64 clock_get_delta_time();

/// Clock functions
///---------------------------------------------------------------------------------------------------------------------

} // End of freya

//////////////////////////////////////////////////////////////////////////

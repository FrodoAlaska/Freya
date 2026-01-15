#include "freya_event.h"
#include "freya_logger.h"
#include "freya_containers.h"

//////////////////////////////////////////////////////////////////////////

namespace freya { // Start of freya

/// ---------------------------------------------------------------------
/// EventEntry
struct EventEntry {
  EventFireFn func; 
  void* listener;
};
/// EventEntry
/// ---------------------------------------------------------------------

/// ---------------------------------------------------------------------
/// EventState
struct EventState {
  using EventPool = DynamicArray<EventEntry>;
  DynamicArray<EventPool> events_pool;
};

static EventState s_state;
/// EventState
/// ---------------------------------------------------------------------

/// ---------------------------------------------------------------------
/// Event functions

void event_init() {
  // Preserve some memory for the pools
  s_state.events_pool.resize(EVENTS_MAX);

  // Done!
  FREYA_LOG_INFO("Event system was successfully initialized");
}

void event_shutdown() {
  // Free-up all of the events

  for(auto& pool : s_state.events_pool) {
    pool.clear();
  }
  s_state.events_pool.clear();

  // Done!
  FREYA_LOG_INFO("Event system was successfully shutdown");
}

void event_register(const EventType type, const EventFireFn& func, const void* listener) {
  DynamicArray<EventEntry>& pool = s_state.events_pool[(sizei)type];
  pool.emplace_back(func, (void*)listener);
}

const bool event_dispatch(const Event& event, const void* dispatcher) {
  // Calling all of the callbacks with the same `event.type` (if they exist)
 
  DynamicArray<EventEntry>& pool = s_state.events_pool[(sizei)event.type];
  for(const auto& entry : pool) {
    if(!entry.func(event, dispatcher, entry.listener)) {
      return false;
    }
  }

  // Done!
  return true;
}

/// Event functions
/// ---------------------------------------------------------------------

} // End of freya

//////////////////////////////////////////////////////////////////////////

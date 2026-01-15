#pragma once

#include "freya_base.h"
#include "freya_containers.h"
#include "freya_pch.h"

//////////////////////////////////////////////////////////////////////////

namespace freya { // Start of freya

/// ----------------------------------------------------------------------
/// ThreadTaskFn

/// The function callback to be invoked when a worker thread 
/// is done with its work.
using ThreadTaskFn = std::function<void()>;

/// ThreadTaskFn
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// ThreadPool 
struct ThreadPool {
  String name; 
  bool is_active;

  DynamicArray<std::thread*> workers; 
  moodycamel::ConcurrentQueue<ThreadTaskFn> tasks;
};
/// ThreadPool 
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// ThreadPool functions

/// Create a thread pool with a given `name`, with `worker_count` amount 
/// of worker threads the `pool` is allowed to have.
FREYA_API void thread_pool_create(ThreadPool& pool, const String& name, const sizei worker_count);

/// Destroy the given `pool`, joining all of its worker threads.
FREYA_API void thread_pool_destroy(ThreadPool& pool);

/// Push the given `task` job to the `pool`'s tasks.
FREYA_API void thread_pool_push_task(ThreadPool& pool, const ThreadTaskFn& task);

/// Retrieve the approximate amount of tasks left.
FREYA_API const sizei thread_pool_get_approx_size(const ThreadPool& pool);

/// ThreadPool functions
/// ----------------------------------------------------------------------

} // End of freya

//////////////////////////////////////////////////////////////////////////

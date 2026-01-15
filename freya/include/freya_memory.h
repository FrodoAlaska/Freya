#pragma once

#include "freya_base.h"
#include "freya_pch.h"

//////////////////////////////////////////////////////////////////////////

namespace freya { // Start of freya

///---------------------------------------------------------------------------------------------------------------------
/// UniquePtr
template<typename T>
using UniquePtr = std::unique_ptr<T>;
/// UniquePtr
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// SharedPtr
template<typename T>
using SharedPtr = std::shared_ptr<T>;
/// SharedPtr
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// WeakPtr
template<typename T>
using WeakPtr = std::weak_ptr<T>;
/// WeakPtr
///---------------------------------------------------------------------------------------------------------------------
 
///---------------------------------------------------------------------------------------------------------------------
/// Memory callbacks

/// A callback that allocates and returns a block of memory with size `size`. 
/// Used for custom allocation purposes.
using AllocateMemoryFn = std::function<void*(const sizei size)>;

/// A callback that frees/reclaims a block of memory `ptr`.
/// Used for custom allocation purposes.
using FreeMemoryFn = std::function<void(void* ptr)>;

/// Memory callbacks
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Memory functions 

/// Allocate a memory block of size `size`.
/// 
/// @NOTE: This function will assert if there's no suffient memory left.
FREYA_API void* memory_allocate(const sizei size);

/// Re-allocate a block of memory `ptr` with a new size of `new_size`.
/// 
/// @NOTE: This function will assert if `ptr` is a `nullptr`.
FREYA_API void* memory_reallocate(void* ptr, const sizei new_size);

/// Set the value of the memory block `ptr` with a size of `ptr_size` to `value`.
/// 
/// @NOTE: This function will assert if `ptr` is a `nullptr`.
FREYA_API void* memory_set(void* ptr, const i32 value, const sizei ptr_size);

/// Set the value of the memory block `ptr` of size `ptr_size` to 0.
/// 
/// @NOTE: This is equivalent to `memory_set(ptr, 0, ptr_size)`.
///
/// @NOTE: This function will assert if `ptr` is a `nullptr`.
FREYA_API void* memory_zero(void* ptr, const sizei ptr_size);

/// Allocate `count` blocks of memory each with the size of `block_size`.
/// 
/// @NOTE: This is equivalent to `memory_allocate(block_size * count)`.
FREYA_API void* memory_blocks_allocate(const sizei count, const sizei block_size);

/// Copy `src_size` bytes of `src` to the memory block `dest`. 
/// 
/// @NOTE: This function will assert if `dest` or `src` are a `nullptr`.
FREYA_API void* memory_copy(void* dest, const void* src, const sizei src_size);

/// Free/reclaim the memory of the given `ptr`.
/// 
/// @NOTE: This function will assert if `ptr` is a `nullptr`.
FREYA_API void memory_free(void* ptr);

/// Retrieve the amount of allocations made so far.
FREYA_API const sizei memory_get_allocations_count();

/// Retrieve the amount of frees made so far. 
FREYA_API const sizei memory_get_frees_count();

/// Retrieve how many bytes have been allocated so far. 
FREYA_API const sizei memory_get_allocation_bytes();

/// Memory functions 
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// @TODO (Memory): Make custom allocaters here...
///---------------------------------------------------------------------------------------------------------------------

} // End of freya

//////////////////////////////////////////////////////////////////////////

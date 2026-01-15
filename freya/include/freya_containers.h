#pragma once

#include "freya_pch.h"

//////////////////////////////////////////////////////////////////////////

namespace freya { // Start of freya

/// ----------------------------------------------------------------------
/// *** Typedefs ***

/// @TODO (Containers): Create your own data structures, you lazy bastard.

using String       = std::string;

template<typename T>
using DynamicArray = std::vector<T>;

template<typename T, sizei N>
using Array        = std::array<T, N>;

template<typename K, typename V> 
using HashMap      = std::unordered_map<K, V>;

template<typename T> 
using Queue        = std::queue<T>;

template<typename T> 
using Stack        = std::stack<T>;

template<typename T> 
using Deque        = std::deque<T>;

/// *** Typedefs ***
/// ----------------------------------------------------------------------

} // End of freya

//////////////////////////////////////////////////////////////////////////

#pragma once

#include "freya_assets.h"

/// ----------------------------------------------------------------------
/// ListSection
struct ListSection {
  freya::AssetType type;
  freya::FilePath directory;

  freya::DynamicArray<freya::FilePath> assets;
};
/// ListSection
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// ListContext 
struct ListContext {
  freya::DynamicArray<ListSection> sections; 
  freya::FilePath parent_dir; 

  lua_State* lua_state = nullptr;
};
/// ListContext 
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// ListContext functions 

bool list_context_load(ListContext& out_list, const freya::FilePath& path);

void list_context_unload(ListContext& list);

/// ListContext context functions 
/// ----------------------------------------------------------------------

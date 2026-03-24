#pragma once

#include "freya_entity.h"

//////////////////////////////////////////////////////////////////////////

namespace freya { // Start of freya

/// ----------------------------------------------------------------------
/// TileMap
struct TileMap {
  EntityWorld* ecs; // @TODO (TileMap): This is super fucking dumb. Don't do this...

  Vec2 tile_size    = Vec2(0.0f);
  IVec2 tiles_count = IVec2(0);

  DynamicArray<Entity> tiles;
};
/// TileMap
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// TileMap functions

FREYA_API void tilemap_create(TileMap& out_map, EntityWorld* ecs, const Vec2& start_position, const Vec2& tile_size, const IVec2& tiles_count);

FREYA_API void tilemap_destroy(TileMap& map);

FREYA_API Entity& tilemap_get_at(TileMap& map, const sizei x_cell, const sizei y_cell);

FREYA_API Entity& tilemap_get_at(TileMap& map, const Vec2& position);

FREYA_API Entity& tilemap_get_neighbor(TileMap& map, const Entity& base_tile, const IVec2& offset);

FREYA_API void tilemap_get_neighbors(TileMap& map, const Entity& base_tile, DynamicArray<Entity>& out_tiles);

FREYA_API void tilemap_select_rect(TileMap& map, const Rect2D& select_box, DynamicArray<Entity>& out_tiles);

FREYA_API void tilemap_place_at(TileMap& map, const sizei x_cell, const sizei y_cell, Entity& tile_entt);

FREYA_API void tilemap_place_at(TileMap& map, const Vec2& position, Entity& tile_entt);

/// TileMap functions
/// ----------------------------------------------------------------------

} // End of freya

//////////////////////////////////////////////////////////////////////////


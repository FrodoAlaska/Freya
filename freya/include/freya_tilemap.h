#pragma once

#include "freya_entity.h"

//////////////////////////////////////////////////////////////////////////

namespace freya { // Start of freya

/// ----------------------------------------------------------------------
/// TileLayer
struct TileLayer {
  String name;
  DynamicArray<Entity> tiles;
};
/// TileLayer
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// TileMap
struct TileMap {
  EntityWorld* ecs; // @TODO (TileMap): This is super fucking dumb. Don't do this...

  Vec2 tile_size    = Vec2(0.0f);
  IVec2 tiles_count = IVec2(0);

  DynamicArray<TileLayer> layers;
};
/// TileMap
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// TileMap functions

FREYA_API void tilemap_create(TileMap& out_map, EntityWorld* ecs, const Vec2& start_position, const Vec2& tile_size, const IVec2& tiles_count);

FREYA_API void tilemap_destroy(TileMap& map);

FREYA_API Vec2 tilemap_index_to_coords(TileMap& map, const sizei x_cell, const sizei y_cell);

FREYA_API IVec2 tilemap_coords_to_index(TileMap& map, const Vec2& coords);

FREYA_API void tilemap_push_layer(TileMap& map, const String& name = "layer_");

FREYA_API void tilemap_pop_layer(TileMap& map);

FREYA_API TileLayer& tilemap_get_layer(TileMap& map, const sizei index);

FREYA_API Entity& tilemap_get_at(TileMap& map, const sizei x_cell, const sizei y_cell, const sizei layer = 0);

FREYA_API Entity& tilemap_get_at(TileMap& map, const Vec2& position, const sizei layer = 0);

FREYA_API Entity& tilemap_get_neighbor(TileMap& map, const Entity& base_tile, const IVec2& offset, const sizei layer = 0);

FREYA_API void tilemap_get_neighbors(TileMap& map, const Entity& base_tile, DynamicArray<Entity>& out_tiles, const sizei layer = 0);

FREYA_API void tilemap_select_rect(TileMap& map, const Rect2D& select_box, DynamicArray<Entity>& out_tiles, const sizei layer = 0);

FREYA_API Entity& tilemap_place_at(TileMap& map, const sizei x_cell, const sizei y_cell, const sizei layer = 0);

FREYA_API Entity& tilemap_place_at(TileMap& map, const Vec2& position, const sizei layer = 0);

/// TileMap functions
/// ----------------------------------------------------------------------

} // End of freya

//////////////////////////////////////////////////////////////////////////


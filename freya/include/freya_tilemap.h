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

/// Initialize the `out_map` with the given information. 
///
/// - `ecs`: A pointer to the `EntityWorld` to manage the internal entities. 
/// - `start_position`: The starting position of the tile map in world coordinates. 
/// - `tile_size`: The size of each tile. 
/// - `tiles_count`: The number of tiles at each axis. The map size in pixels can be determined by calculating `tile_size * tiles_count`.
///
FREYA_API void tilemap_create(TileMap& out_map, EntityWorld* ecs, const Vec2& start_position, const Vec2& tile_size, const IVec2& tiles_count);

/// Destroy and reclaim any memory consumed by `map`.
FREYA_API void tilemap_destroy(TileMap& map);

/// Convert the given `x_cell` and `y_cell` indices to world coordinates.
FREYA_API Vec2 tilemap_index_to_coords(TileMap& map, const sizei x_cell, const sizei y_cell);

/// Convert the given `coords` world coordinates to indices.
FREYA_API IVec2 tilemap_coords_to_index(TileMap& map, const Vec2& coords);

/// Push a new tile layer into `map`, using `name` as an identifier.
FREYA_API void tilemap_push_layer(TileMap& map, const String& name = "layer_");

/// Pop the last layer of the given `map`.
///
/// @NOTE: Make sure that `map.layer` is not empty before carrying out this operation.
FREYA_API void tilemap_pop_layer(TileMap& map);

/// Retrieve the layer at `index`.
FREYA_API TileLayer& tilemap_get_layer(TileMap& map, const sizei index);

/// Retrieve the tile entity at `x_cell` and `y_cell` in `layer` index.
FREYA_API Entity& tilemap_get_at(TileMap& map, const sizei x_cell, const sizei y_cell, const sizei layer = 0);

/// Retrieve the tile entity at found at `position` woorld coordinates in `layer` index.
FREYA_API Entity& tilemap_get_at(TileMap& map, const Vec2& position, const sizei layer = 0);

/// Retrieve the neighbor tile at `offset` from `base_tile` in `layer`.
FREYA_API Entity& tilemap_get_neighbor(TileMap& map, const Entity& base_tile, const IVec2& offset, const sizei layer = 0);

/// Retrieve all adjacent neighbors of the given `base_tile` in `layer` index, writing the result to `out_tiles`.
FREYA_API void tilemap_get_neighbors(TileMap& map, const Entity& base_tile, DynamicArray<Entity>& out_tiles, const sizei layer = 0);

/// Select all positions (that then can be used to retrieve the tiles at those positions) within 
/// the given `select_box` in `layer` index, writing the result to `out_tiles`.
FREYA_API void tilemap_select_rect(TileMap& map, const Rect2D& select_box, DynamicArray<Vec2>& out_tiles, const sizei layer = 0);

/// Create and place a new tile entity at `x_cell` and `y_cell` in `layer` index.
FREYA_API Entity& tilemap_place_at(TileMap& map, const sizei x_cell, const sizei y_cell, const sizei layer = 0);

/// Create and place a new tile entity at `position` world coordinates in `layer` index.
FREYA_API Entity& tilemap_place_at(TileMap& map, const Vec2& position, const sizei layer = 0);

/// TileMap functions
/// ----------------------------------------------------------------------

} // End of freya

//////////////////////////////////////////////////////////////////////////


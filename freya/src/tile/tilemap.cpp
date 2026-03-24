#include "freya_tilemap.h"

//////////////////////////////////////////////////////////////////////////

namespace freya { // Start of freya

/// ----------------------------------------------------------------------
/// Consts

const sizei TILE_NEIGHBORS_MAX = 9;

/// Consts
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// TileMap functions

void tilemap_create(TileMap& out_map, EntityWorld* ecs, const Vec2& start_position, const Vec2& tile_size, const IVec2& tiles_count) {
  out_map.tile_size   = tile_size;
  out_map.tiles_count = tiles_count;
  out_map.ecs         = ecs;

  out_map.tiles.resize(out_map.tiles_count.x * out_map.tiles_count.y);
  for(freya::i32 i = 0; i < out_map.tiles_count.x; i++) {
    for(freya::i32 j = 0; j < out_map.tiles_count.y; j++) {
      Vec2 position = start_position + (out_map.tile_size * Vec2(i, j));

      Entity& tile = tilemap_get_at(out_map, position);
      tile         = entity_create(*out_map.ecs, position);
    }
  }
}

void tilemap_destroy(TileMap& map) {
  for(auto& tile : map.tiles) {
    entity_destroy(*map.ecs, tile);
  }
  map.tiles.clear();
}

Entity& tilemap_get_at(TileMap& map, const sizei x_cell, const sizei y_cell) {
  /// @NOTE:
  ///
  /// Even if the index is out of bounds, we simply return the tile at that edge. 
  /// That might cause harm in the future. If that's the case, we can change it to 
  /// be an assert instead, or just return an ENTITY_NULL.
  ///

  sizei index = y_cell * map.tiles_count.x + x_cell;
  return map.tiles[clamp_int(index, 0, map.tiles.size() - 1)];
}

Entity& tilemap_get_at(TileMap& map, const Vec2& position) {
  Vec2 clamped_pos = vec2_clamp(position, Vec2(0.0f), map.tile_size * (Vec2)(map.tiles_count - 1)); 
  IVec2 index      = (IVec2)(clamped_pos / map.tile_size);

  return tilemap_get_at(map, index.x, index.y);
}

Entity& tilemap_get_neighbor(TileMap& map, const Entity& base_tile, const IVec2& offset) {
  const Vec2& tile_pos = entity_get_component_const<Transform>(*map.ecs, base_tile).position;
  IVec2 tile_index     = (IVec2)(tile_pos / map.tile_size) - offset;

  return tilemap_get_at(map, tile_index.x, tile_index.y);
}

void tilemap_get_neighbors(TileMap& map, const Entity& base_tile, DynamicArray<Entity>& out_tiles) {
  out_tiles.clear();
  for(i32 i = -1; i <= 1; i++) {
    for(i32 j = -1; j <= 1; j++) {
      out_tiles.push_back(tilemap_get_neighbor(map, base_tile, IVec2(i, j)));
    }
  }
}

void tilemap_select_rect(TileMap& map, const Rect2D& select_box, DynamicArray<Entity>& out_tiles) {
  out_tiles.clear();
  IVec2 count = (IVec2)(select_box.size / map.tile_size);

  for(i32 i = 0; i < count.x; i++) {
    for(i32 j = 0; j < count.y; j++) {
      Vec2 position = select_box.position + (map.tile_size * Vec2(i, j));
      out_tiles.push_back(tilemap_get_at(map, position));
    }
  }
}

void tilemap_place_at(TileMap& map, const sizei x_cell, const sizei y_cell, Entity& tile_entt) {
  /// @NOTE:
  ///
  /// This looks a little weird, but I can explain.
  ///
  /// Basically, the `tilemap_get_at` function returns a reference 
  /// to the entity at that index. Which means that we can 
  /// directly change that reference to another entity, propagating 
  /// the changes in the array itself. 
  /// Essentially, we're just changing one entity reference to another. 
  ///
  /// I saw it once somewhere and I thought it was cool. Whatever. Might change later.
  ///
  tilemap_get_at(map, x_cell, y_cell) = tile_entt;
}

void tilemap_place_at(TileMap& map, const Vec2& position, Entity& tile_entt) {
  tilemap_get_at(map, position) = tile_entt;
}

/// TileMap functions
/// ----------------------------------------------------------------------

} // End of freya

//////////////////////////////////////////////////////////////////////////


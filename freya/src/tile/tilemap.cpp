#include "freya_tilemap.h"
#include "freya_logger.h"

//////////////////////////////////////////////////////////////////////////

namespace freya { // Start of freya

/// ----------------------------------------------------------------------
/// TileMap functions

void tilemap_create(TileMap& out_map, EntityWorld* ecs, const Vec2& start_position, const Vec2& tile_size, const IVec2& tiles_count) {
  out_map.tile_size   = tile_size;
  out_map.tiles_count = tiles_count;
  out_map.ecs         = ecs;
}

void tilemap_destroy(TileMap& map) {
  for(auto& layer : map.layers) {
    for(auto& tile : layer.tiles) {
      if(tile == ENTITY_NULL) {
        continue;
      }

      entity_destroy(*map.ecs, tile);
    }
    layer.tiles.clear();
  }
  map.layers.clear();
}

Vec2 tilemap_index_to_coords(TileMap& map, const sizei x_cell, const sizei y_cell) {
  Vec2 clamped_index = vec2_clamp(Vec2(x_cell, y_cell), Vec2(0.0f), (Vec2)(map.tiles_count - 1));
  return clamped_index * map.tile_size; 
}

IVec2 tilemap_coords_to_index(TileMap& map, const Vec2& coords) {
  Vec2 clamped_pos = vec2_clamp(coords, Vec2(0.0f), map.tile_size * (Vec2)(map.tiles_count - 1)); 
  return (IVec2)(clamped_pos / map.tile_size);
}

void tilemap_push_layer(TileMap& map, const String& name) {
  TileLayer& layer = map.layers.emplace_back(name);
  layer.tiles.resize(map.tiles_count.x * map.tiles_count.y);
}

void tilemap_pop_layer(TileMap& map) {
  // Destroy the tiles within the layer

  TileLayer& layer = map.layers.back();
  for(auto& tile : layer.tiles) {
    if(tile == ENTITY_NULL) {
      continue;
    }

    entity_destroy(*map.ecs, tile);
  }
 
  // Pop the layer completely

  layer.tiles.clear();
  map.layers.pop_back();
}

TileLayer& tilemap_get_layer(TileMap& map, const sizei index) {
  FREYA_DEBUG_ASSERT((index >= 0 && index < map.layers.size()), "Invalid tile layer index");
  return map.layers[index];
}

Entity& tilemap_get_at(TileMap& map, const sizei x_cell, const sizei y_cell, const sizei layer) {
  /// @NOTE:
  ///
  /// Even if the index is out of bounds, we simply return the tile at that edge. 
  /// That might cause harm in the future. If that's the case, we can change it to 
  /// be an assert instead, or just return an ENTITY_NULL.
  ///

  sizei index = y_cell * map.tiles_count.x + x_cell;
  return map.layers[layer].tiles[clamp_int(index, 0, map.layers[layer].tiles.size() - 1)];
}

Entity& tilemap_get_at(TileMap& map, const Vec2& position, const sizei layer) {
  IVec2 index = tilemap_coords_to_index(map, position);
  return tilemap_get_at(map, index.x, index.y, layer);
}

Entity& tilemap_get_neighbor(TileMap& map, const Entity& base_tile, const IVec2& offset, const sizei layer) {
  Vec2 offset_pos      = map.tile_size * (Vec2)offset;
  const Vec2& tile_pos = entity_get_component_const<Transform>(*map.ecs, base_tile).position - offset_pos;

  return tilemap_get_at(map, tile_pos, layer);
}

void tilemap_get_neighbors_immediate(TileMap& map, const Entity& base_tile, Array<Entity, 4>& out_tiles, const sizei layer) {
  /// @NOTE: 
  ///
  /// Doing it by hand because I'm lazy and hate double for-loops...
  /// Also, formatting it because it just looks nice lol.
  ///

  out_tiles[0] = tilemap_get_neighbor(map, base_tile, IVec2( 0, -1), layer); // Top
  out_tiles[1] = tilemap_get_neighbor(map, base_tile, IVec2(-1,  0), layer); // Left
  out_tiles[2] = tilemap_get_neighbor(map, base_tile, IVec2( 0, -1), layer); // Bottom
  out_tiles[3] = tilemap_get_neighbor(map, base_tile, IVec2(-1,  0), layer); // Right
}

void tilemap_get_neighbors_all(TileMap& map, const Entity& base_tile, Array<Entity, 8>& out_tiles, const sizei layer) {
  /// @NOTE: 
  ///
  /// Just the same story...
  ///
  
  out_tiles[0] = tilemap_get_neighbor(map, base_tile, IVec2(-1    ), layer); // Top-left
  out_tiles[1] = tilemap_get_neighbor(map, base_tile, IVec2( 0, -1), layer); // Top-center
  out_tiles[2] = tilemap_get_neighbor(map, base_tile, IVec2( 1    ), layer); // Top-right
  out_tiles[3] = tilemap_get_neighbor(map, base_tile, IVec2(-1,  0), layer); // Center-left
  out_tiles[4] = tilemap_get_neighbor(map, base_tile, IVec2( 1,  0), layer); // Center-right
  out_tiles[5] = tilemap_get_neighbor(map, base_tile, IVec2(-1,  1), layer); // Bottom-left
  out_tiles[6] = tilemap_get_neighbor(map, base_tile, IVec2( 0,  1), layer); // Bottom-center
  out_tiles[7] = tilemap_get_neighbor(map, base_tile, IVec2( 0, -1), layer); // Bottom-right
}

void tilemap_select_rect(TileMap& map, const Rect2D& select_box, DynamicArray<Vec2>& out_tiles, const sizei layer) {
  out_tiles.clear();
  IVec2 count = (IVec2)(select_box.size / map.tile_size);

  for(i32 i = 0; i < count.x; i++) {
    for(i32 j = 0; j < count.y; j++) {
      Vec2 position = select_box.position + (map.tile_size * Vec2(i, j));
      out_tiles.push_back(position);
    }
  }
}

Entity& tilemap_place_at(TileMap& map, const sizei x_cell, const sizei y_cell, const sizei layer) {
  Entity& entt = tilemap_get_at(map, x_cell, y_cell, layer);
  entt         = entity_create(*map.ecs, tilemap_index_to_coords(map, x_cell, y_cell));

  return entt;
}

Entity& tilemap_place_at(TileMap& map, const Vec2& position, const sizei layer) {
  IVec2 index = tilemap_coords_to_index(map, position);
  return tilemap_place_at(map, index.x, index.y, layer);
}

/// TileMap functions
/// ----------------------------------------------------------------------

} // End of freya

//////////////////////////////////////////////////////////////////////////


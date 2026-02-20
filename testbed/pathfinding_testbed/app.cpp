#include "app.h"

#include <freya.h>
#include <imgui/imgui.h>

/// ----------------------------------------------------------------------
/// Consts

const freya::sizei PATH_MAX_NEIGHBORS = 4;

const freya::sizei PATH_NODES_MAX = 32;
const freya::f32 PATH_NODE_SIZE   = 32.0f;

const freya::u32 INF = ((freya::u32)-1);

/// Consts
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// PathNode
struct PathNode {
  freya::u32 h = INF; 
  freya::u32 g = INF;
  freya::u32 f = INF;

  bool is_traversable = true; 
  bool is_visited     = false;

  freya::IVec2 position = freya::IVec2(0);

  PathNode* parent = nullptr;
  PathNode* neighbors[PATH_MAX_NEIGHBORS];
};
/// PathNode
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// PathMap
struct PathMap {
  PathNode nodes[PATH_NODES_MAX][PATH_NODES_MAX];

  PathNode* start = nullptr; 
  PathNode* end   = nullptr;

  freya::DynamicArray<PathNode*> path;

  freya::List<PathNode*> open_nodes;
  freya::List<PathNode*> closed_nodes;
};
/// PathMap
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Private functions

static freya::u32 manhattan_dist(const freya::IVec2& v1, const freya::IVec2& v2) {
  return (freya::u32)(freya::abs(v1.x - v2.x) + freya::abs(v1.y - v2.y));
}

/// Private functions
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// PathMap functions

void pathmap_create(PathMap& map) {
  // Nodes init

  for(freya::sizei i = 0; i < PATH_NODES_MAX; i++) {
    for(freya::sizei j = 0; j < PATH_NODES_MAX; j++) {
      PathNode* node = &map.nodes[i][j];
      node->position = freya::IVec2(i, j);

      // Assign the neighbors of every node 

      if(i > 0) { // Left
        node->neighbors[0] = &map.nodes[i - 1][j];
      }
      if(i < PATH_NODES_MAX) { // Right
        node->neighbors[1] = &map.nodes[i + 1][j];
      }

      if(j > 0) { // Top
        node->neighbors[2] = &map.nodes[i][j - 1];
      }
      if(j < PATH_NODES_MAX) { // Bottom
        node->neighbors[3] = &map.nodes[i][j + 1];
      }
    }
  }

  // Performance improvement?
  map.path.reserve(16);
}

void pathmap_reset(PathMap& map) {
  // Reset the state of each node
  
  for(freya::sizei i = 0; i < PATH_NODES_MAX; i++) {
    for(freya::sizei j = 0; j < PATH_NODES_MAX; j++) {
      PathNode* node = &map.nodes[i][j];

      node->h = INF;
      node->g = INF;
      node->f = INF;
     
      node->is_visited = false;
      node->parent     = nullptr;
    }
  }

  // Reset internal arrays
  
  map.open_nodes.clear();
  map.path.clear();
}

void pathmap_calculate(PathMap& map) {
  // We don't have any start or end nodes to process

  if(!map.start || !map.end) {
    return;
  }

  // Reset the whole map
  pathmap_reset(map);
  
  // Setup the starting node

  map.start->g = 0;
  map.start->h = manhattan_dist(map.start->position, map.end->position);
  map.start->f = map.start->g + map.start->h;
  map.open_nodes.push_front(map.start);

  // Go through all the possible nodes

  PathNode* current = map.start;
  while(!map.open_nodes.empty() && current != map.end) {
    // Re-sort the list

    map.open_nodes.sort([](const PathNode* lhs, const PathNode* rhs) {
      return lhs->f < rhs->f;
    });

    // Pop every node that has been processed already. 

    while(!map.open_nodes.empty() && map.open_nodes.front()->is_visited) {
      map.open_nodes.pop_front();
    }

    // No more nodes to process... abort!

    if(map.open_nodes.empty()) {
      break;
    }

    // Visting the node at the front of the list

    current             = map.open_nodes.front();
    current->is_visited = true;

    // Step through the neighbors of the current node
    
    for(auto& neighbor : current->neighbors) {
      // Invalid neighbor

      if(!neighbor) {
        continue;
      }

      if(!neighbor->is_traversable || neighbor->is_visited) {
        continue;
      }

      // Calculate the G cost of the neighbor, and see if we 
      // can update it or not

      freya::u32 move_cost = current->g + manhattan_dist(map.start->position, neighbor->position);
      if(move_cost > neighbor->g) { // We're not responsible for updating this neighbor... skip
        continue;
      }

      // Calculate the G, H, and F costs for the neighbor

      neighbor->g = move_cost; 
      neighbor->h = manhattan_dist(neighbor->position, map.end->position);
      neighbor->f = neighbor->g + neighbor->h;

      neighbor->parent = current;

      // Add this neighbor to be processed later
      map.open_nodes.push_front(neighbor); 
    }
  }

  // Traverse the list and add the path accordingly

  current = map.end;
  while(current->parent) {
    map.path.emplace_back(current);
    current = current->parent;
  }
}

PathNode* pathmap_get_node(PathMap& map, const freya::IVec2& coords) {
  freya::IVec2 index = coords / (freya::i32)PATH_NODE_SIZE;
  return &map.nodes[index.x][index.y];
}

PathNode* pathmap_pick_node(PathMap& map, freya::Camera& camera) {
  freya::IVec2 mouse_pos = freya::camera_world_to_screen_space(camera, freya::input_mouse_position() + (PATH_NODE_SIZE / 2.0f));
  return pathmap_get_node(map, mouse_pos);
}

void pathmap_process_input(PathMap& map, freya::Camera& camera) {
  bool has_changed = false;

  // Pick the nodes 
  
  if(freya::input_key_down(freya::KEY_LEFT_SHIFT) && freya::input_button_pressed(freya::MOUSE_BUTTON_LEFT)) { // Pick the end node
    map.end = pathmap_pick_node(map, camera);

    has_changed = true;
  }
  else if(freya::input_key_down(freya::KEY_LEFT_CONTROL) && freya::input_button_pressed(freya::MOUSE_BUTTON_LEFT)) { // Block the node
    PathNode* node       = pathmap_pick_node(map, camera);
    node->is_traversable = !node->is_traversable;
    
    has_changed = true;
  }
  else if(freya::input_button_pressed(freya::MOUSE_BUTTON_LEFT)) { // Pick ths start node
    map.start = pathmap_pick_node(map, camera);
    
    has_changed = true;
  }

  // Re-calculate the algorithm if any changes occured

  if(has_changed) {
    pathmap_calculate(map);
  }
}

void pathmap_render(PathMap& map) {
  // Render each tile

  for(freya::sizei i = 0; i < PATH_NODES_MAX; i++) {
    for(freya::sizei j = 0; j < PATH_NODES_MAX; j++) {
      PathNode* node = &map.nodes[i][j];

      freya::Color color = freya::COLOR_GREEN;
      if(!node->is_traversable) {
        color = freya::COLOR_BLACK;
      }

      freya::Transform transform = {
        .position = node->position * (freya::i32)PATH_NODE_SIZE,
        .scale    = freya::Vec2(PATH_NODE_SIZE),
      };
      freya::renderer_queue_quad(transform, color);
    }
  }

  // Render the paths

  for(auto& node : map.path) {
    freya::Transform transform = {
      .position = node->position * (freya::i32)PATH_NODE_SIZE,
      .scale    = freya::Vec2(PATH_NODE_SIZE),
    };
    freya::renderer_queue_quad(transform, freya::COLOR_WHITE);
  }

  // Render the start point

  if(map.start) {
    freya::Transform transform = {
      .position = map.start->position * (freya::i32)PATH_NODE_SIZE,
      .scale    = freya::Vec2(PATH_NODE_SIZE),
    };
    freya::renderer_queue_quad(transform, freya::COLOR_BLUE);
  }

  // Render the end point

  if(map.end) {
    freya::Transform transform = {
      .position = map.end->position * (freya::i32)PATH_NODE_SIZE,
      .scale    = freya::Vec2(PATH_NODE_SIZE),
    };
    freya::renderer_queue_quad(transform, freya::COLOR_RED);
  }
}

/// PathMap functions
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// App
struct freya::App {
  freya::Window* window;
  freya::Camera camera;
  freya::AssetGroupID group_id;

  PathMap path;
};
/// App
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// App functions 

freya::App* app_init(const freya::Args& args, freya::Window* window) {
  // App init
  
  freya::App* app = new freya::App{};
  freya::renderer_set_clear_color(freya::Vec4(0.1f, 0.1f, 0.1f, 1.0f));

  // Window init
  app->window = window;

  // Editor init
  freya::gui_init(window);

  // Camera init
  
  freya::CameraDesc cam_desc = {
    .position = freya::Vec2(0.0f),
    .zoom     = 1.0f,
  };
  freya::camera_create(app->camera, cam_desc);

  // Assets init

  app->group_id = freya::asset_group_create("app_assets");
  
  freya::asset_group_build(app->group_id, "../../assets/asset_list.frlist", "assets.frpkg");
  freya::asset_group_load_package(app->group_id, "assets.frpkg");

  // UI asset init

  freya::ui_renderer_set_asset_group(app->group_id);
  freya::ui_renderer_set_font("HeavyDataNerdFont");

  // Path map init
  pathmap_create(app->path);

  // Done!
  return app;
}

void app_shutdown(freya::App* app) {
  freya::asset_group_destroy(app->group_id);
  freya::gui_shutdown();

  delete app;
}

void app_update(freya::App* app, const freya::f32 delta_time) {
  // Quit the application when the specified exit key is pressed
  
  if(freya::input_key_pressed(freya::KEY_ESCAPE)) {
    freya::event_dispatch(freya::Event{.type = freya::EVENT_APP_QUIT});
    return;
  }

  // Disable/enable the GUI
  
  if(freya::input_key_pressed(freya::KEY_F1)) {
    freya::gui_toggle_active();
    freya::physics_world_toggle_debug();
  }

  // Map update
  pathmap_process_input(app->path, app->camera);
}

void app_render(freya::App* app) {
  // 2D render

  freya::renderer_begin(app->camera);
  pathmap_render(app->path);
  freya::renderer_end();

  // UI render
  
  freya::ui_renderer_begin();
  freya::ui_renderer_end();
}

void app_render_gui(freya::App* app) {
  if(!freya::gui_is_active()) {
    return;
  }

  freya::gui_begin(); 
 
  // Debug
  freya::gui_debug_info();

  // Editor

  freya::gui_begin_panel("Editor");
  freya::gui_edit_camera("Camera", &app->camera);
  freya::gui_end_panel();

  freya::gui_end();
}

/// App functions 
/// ----------------------------------------------------------------------

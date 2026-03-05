#include "freya_math.h"

//////////////////////////////////////////////////////////////////////////

namespace freya { // Start of freya

///---------------------------------------------------------------------------------------------------------------------
/// Consts

const f32 SQRT_2           = 1.41421356237; // Result of sqrt(2.0f)
const sizei MAX_ITERATIONS = 1000;

/// Consts
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// PoissonDisk 
struct PoissonDisk {
  PoissonDiskDesc desc = {};
  f32 cell_size = 0.0f;
 
  DynamicArray<DynamicArray<i32>> grid;
  DynamicArray<Vec2> active_points;
};
/// PoissonDisk 
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Private functions

static bool point_is_valid(PoissonDisk& disk, DynamicArray<Vec2>& out_points, const Vec2 point) {
  // Point should be whithin the region

  if(!point_in_rect(point, Vec2(0.0f), disk.desc.region_size)) {
    return false;
  }
  
  // Look at the surrounding cells and check for validity in each one

  IVec2 cell = (IVec2)(point / disk.cell_size);

  IVec2 search_start = vec2_max(IVec2(0), cell - 2);
  IVec2 search_end   = vec2_min(cell + 2, Vec2((f32)disk.grid.size(), (f32)disk.grid[0].size()) - 2.0f);

  bool is_valid = true;

  for(i32 x = search_start.x; x <= search_end.x; x++) {
    for(i32 y = search_start.y; y <= search_end.y; y++) {
      i32 index = disk.grid[x][y];

      // The cell has not been visited yet, so ignore it

      if(index == -1) {
        continue;
      }

      // Make sure that the current point does not overlap with the candidate
       
      if(point_in_circle(out_points[index], point, disk.desc.radius)) {
        is_valid = false;
        break;
      }
    }
  }

  return is_valid;
}

/// Private functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// PoissonDisk functions

void poisson_disk_calculate(const PoissonDiskDesc& desc, DynamicArray<Vec2>& out_points) {
  // Poisson state init 

  PoissonDisk disk;
  disk.desc      = desc;
  disk.cell_size = desc.radius / SQRT_2;

  // Grid init

  disk.grid.resize((i32)(desc.region_size.x / disk.cell_size));
  for(auto& y_grid : disk.grid) {
    y_grid.resize((i32)(desc.region_size.y / disk.cell_size));
    for(auto& cell : y_grid) {
      cell = -1;
    }
  }

  // Set up the arrays

  out_points.clear();

  out_points.reserve(256); // @TEMP: Do we need this?
  disk.active_points.reserve(256);

  // Run through the active points until it's empty

  disk.active_points.push_back(desc.start_point);
  while(disk.active_points.size() > 0) {
    i32 random_index = random_i32(0, disk.active_points.size() - 1);
    Vec2 point       = disk.active_points[random_index];

    // Iterate through _k_ number of samples, until we either a) find a valid point, or 
    // b) we ran out of samples to test.

    bool found = false;
    for(i32 i = 0; i < desc.num_iterations; i++) {
      f32 random_angle = random_f32(0.0f, 1.0f) * freya::PI * 2;
      Vec2 angle_dir   = Vec2(freya::cos(random_angle), freya::sin(random_angle));

      // Make sure that the candidate is actually valid

      Vec2 candidate = point + angle_dir * random_f32(desc.radius, desc.radius * 2.0f);
      if(!point_is_valid(disk, out_points, candidate)) {
        continue;
      }

      // Make sure that the candidate is in a valid grid area

      i32 column = (i32)(candidate.x / disk.cell_size);
      i32 row    = (i32)(candidate.y / disk.cell_size);

      if((column < 0 || column > (disk.grid.size() - 1)) || (row < 0 || row > (disk.grid.size() - 1))) {
        continue;
      }

      // We found a candidate! Now, add it to the list of active points
       
      found = true; 

      disk.active_points.push_back(candidate);
      out_points.push_back(candidate);

      disk.grid[column][row] = (freya::i32)(out_points.size() - 1);

      // We only care about finding _one_ point, so there's no need to keep going 
      break;
    }

    // @TEMP: 
    // Stop after a few iterations, since the loop keeps going apparently. 
    // I'll figure it out later...
    
    if(out_points.size() >= MAX_ITERATIONS) {
      break;
    }

    // The point was not found. Remove it from the array
     
    if(!found) {
      disk.active_points.erase(disk.active_points.begin() + random_index);
    }
  }
}

/// PoissonDisk functions
///---------------------------------------------------------------------------------------------------------------------

} // End of freya

//////////////////////////////////////////////////////////////////////////

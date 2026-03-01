#pragma once

#include "freya_math.h"

/// @NOTE: 
///
/// This is an abstraction over the `FastNoiseLite` library. Most 
/// of the functions and types here mirror that of `FastNoiseLite`, 
/// with only a few changes. 
///

//////////////////////////////////////////////////////////////////////////

namespace freya { // Start of freya

///---------------------------------------------------------------------------------------------------------------------
/// NoiseType
enum NoiseType {
  NOISE_OPEN_SIMPLEX2,
  NOISE_OPEN_SIMPLEX2S,
  NOISE_CELLULAR,
  NOISE_PERLIN,
  NOISE_VALUE_CUBIC,
  NOISE_VALUE,
};
/// NoiseType
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// RotationType
enum RotationType {
  ROTATION_NONE, 
  ROTATION_XY_PLANES,
  ROTATION_XZ_PLANES,
};
/// RotationType
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// FractalType
enum FractalType {
  FRACTAL_NONE,
  FRACTAL_FBM,
  FRACTAL_RIDGED,
  FRACTAL_PING_PONG,
  FRACTAL_DOMAIN_WARP_PROGRESSIVE,
  FRACTAL_DOMAIN_WARP_INDEPENDENT,
};
/// FractalType
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// CellularDistanceType
enum CellularDistanceType {
  CELLULAR_DISTANCE_EUCLIDEAN,
  CELLULAR_DISTANCE_EUCLIDEAN_SQ,
  CELLULAR_DISTANCE_MANHATTAN,
  CELLULAR_DISTANCE_HYBRID,
};
/// CellularDistanceType
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// CellularReturnType
enum CellularReturnType {
  CELLULAR_RETURN_CELL_VALUE, 
  CELLULAR_RETURN_DISTANCE, 
  CELLULAR_RETURN_DISTANCE2, 
  CELLULAR_RETURN_DISTANCE2_ADD, 
  CELLULAR_RETURN_DISTANCE2_SUB, 
  CELLULAR_RETURN_DISTANCE2_MUL, 
  CELLULAR_RETURN_DISTANCE2_DIV, 
};
/// CellularReturnType
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// DomainWarpType
enum DomainWarpType {
  DOMAIN_WARP_OPEN_SIMPLEX2,
  DOMAIN_WARP_OPEN_SIMPLEX2_REDUCED,
  DOMAIN_WARP_OPEN_BASIC_GRID,
};
/// DomainWarpType
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// NoiseGeneratorDesc
struct NoiseGeneratorDesc {
  NoiseType noise_type       = NOISE_OPEN_SIMPLEX2; 
  RotationType rotation_type = ROTATION_NONE;
  FractalType fractal_type   = FRACTAL_NONE;

  CellularDistanceType distance_func = CELLULAR_DISTANCE_EUCLIDEAN_SQ;
  CellularReturnType return_type     = CELLULAR_RETURN_DISTANCE;
  DomainWarpType warp_type           = DOMAIN_WARP_OPEN_SIMPLEX2;

  i32 seed    = 1337;
  i32 octaves = 3;

  f32 frequency  = 0.01f; 
  f32 lacunarity = 2.0f;
  f32 gain       = 0.5f;
  f32 strength   = 0.0f; 

  f32 ping_pong_strength = 2.0f;
  f32 cellular_jitter    = 1.0f;
  f32 domain_amp         = 1.0f;
};
/// NoiseGeneratorDesc
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// NoiseGenerator
struct NoiseGenerator;
/// NoiseGenerator
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// NoiseGenerator functions

FREYA_API NoiseGenerator* noise_generator_create(const NoiseGeneratorDesc& desc);

FREYA_API void noise_generator_destroy(NoiseGenerator* gen);

FREYA_API void noise_generator_set_desc(NoiseGenerator* gen, const NoiseGeneratorDesc& desc);

FREYA_API NoiseGeneratorDesc& noise_generator_get_desc(NoiseGenerator* gen);

FREYA_API f32 noise_generator_get(NoiseGenerator* gen, const Vec2& coords);

FREYA_API f32 noise_generator_get(NoiseGenerator* gen, const Vec3& coords);

FREYA_API void noise_generator_domain_warp(NoiseGenerator* gen, Vec2& out_coords);

FREYA_API void noise_generator_domain_warp(NoiseGenerator* gen, Vec3& out_coords);

/// NoiseGenerator functions
///---------------------------------------------------------------------------------------------------------------------

} // End of freya

//////////////////////////////////////////////////////////////////////////

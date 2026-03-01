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
  /// The noise type of this generator.
  ///
  /// @NOTE: The default is `NOISE_OPEN_SIMPLEX2`.
  NoiseType noise_type       = NOISE_OPEN_SIMPLEX2; 
  
  /// The rotation type of this generator if 3D noise 
  /// is being used.
  ///
  /// @NOTE: The default is `ROTATION_NONE`.
  RotationType rotation_type = ROTATION_NONE;
  
  /// The fractal type of this generator.
  ///
  /// @NOTE: The default is `FRACTAL_NONE`.
  FractalType fractal_type   = FRACTAL_NONE;

  /// The cellular distance function that will be used 
  /// if the noise type is set to `NOISE_CELLULAR`.
  ///
  /// @NOTE: The default is `CELLULAR_DISTANCE_EUCLIDEAN_SQ`.
  CellularDistanceType distance_func = CELLULAR_DISTANCE_EUCLIDEAN_SQ;
  
  /// The cellular return type that will be used 
  /// if the noise type is set to `NOISE_CELLULAR`.
  ///
  /// @NOTE: The default is `CELLULAR_RETURN_DISTANCE`.
  CellularReturnType return_type     = CELLULAR_RETURN_DISTANCE;
  
  /// The domain warp type that will be used if the 
  /// fractal type is set to any of the `FRACTAL_DOMAIN_WARP_*` values.
  ///
  /// @NOTE: The default is `DOMAIN_WARP_OPEN_SIMPLEX2`.
  DomainWarpType warp_type           = DOMAIN_WARP_OPEN_SIMPLEX2;

  /// The seed value of this generator. 
  /// Each seed will have a unique noise output. 
  ///
  /// @NOTE: The default is set to `1337`.
  i32 seed    = 1337;

  /// The number of octaves of this generator.
  ///
  /// @NOTE: The default is set to `3`.
  i32 octaves = 3;

  /// The frequency of fractals of this generator.
  ///
  /// @NOTE: The default is set to `0.01f`.
  f32 frequency  = 0.01f; 
  
  /// The lacunarity value of fractals of this generator.
  ///
  /// @NOTE: The default is set to `2.0f`.
  f32 lacunarity = 2.0f;

  /// The amount of gain fractals will have at each octave.
  ///
  /// @NOTE: The default is set to `0.5f`.
  f32 gain       = 0.5f;

  /// The weighted strength of the fractals.
  ///
  /// @NOTE: The default is set to `0.0f`.
  f32 strength   = 0.0f; 

  /// The strength of the ping pong effect when using 
  /// the `FRACTAL_PING_PONG` type.
  ///
  /// @NOTE: The default is set to `2.0f`.
  f32 ping_pong_strength = 2.0f;
 
  /// The maximum jitter (distance) a cellular point can travel 
  /// from its initial position.
  ///
  /// @NOTE: The default is set to `1.0f`.
  f32 cellular_jitter    = 1.0f;

  /// The maximum warp distance (amplifier) from the initial position 
  /// when using the `noise_generator_domain_warp` functions.
  ///
  /// @NOTE: The default is set to `1.0f`.
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

/// Allocate and create a noise generator object, using the information given in `desc`.
FREYA_API NoiseGenerator* noise_generator_create(const NoiseGeneratorDesc& desc);

/// Destroy the given `gen` object, reclaiming any allocated memory.
FREYA_API void noise_generator_destroy(NoiseGenerator* gen);

/// Set the internal `NoiseGeneratorDesc` of `gen`, setting up the generator for use.
FREYA_API void noise_generator_set_desc(NoiseGenerator* gen, const NoiseGeneratorDesc& desc);

/// Retrieve the internal `NoiseGeneratorDesc` of the given `gen`.
FREYA_API NoiseGeneratorDesc& noise_generator_get_desc(NoiseGenerator* gen);

/// Retrive the 2D noise value between [-1, 1] at `coords`, using the information provided in `gen`.
FREYA_API f32 noise_generator_get(NoiseGenerator* gen, const Vec2& coords);

/// Retrive the 3D noise value between [-1, 1] at `coords`, using the information provided in `gen`.
FREYA_API f32 noise_generator_get(NoiseGenerator* gen, const Vec3& coords);

/// Wraps the 2D input position, writing the values to `out_coords`, using the information provided in `gen`.
FREYA_API void noise_generator_domain_warp(NoiseGenerator* gen, Vec2& out_coords);

/// Wraps the 3D input position, writing the values to `out_coords`, using the information provided in `gen`.
FREYA_API void noise_generator_domain_warp(NoiseGenerator* gen, Vec3& out_coords);

/// NoiseGenerator functions
///---------------------------------------------------------------------------------------------------------------------

} // End of freya

//////////////////////////////////////////////////////////////////////////

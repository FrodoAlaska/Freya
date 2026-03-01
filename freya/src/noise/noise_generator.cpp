#include "freya_noise.h"
#include "freya_logger.h"

#include <FastNoiseLite/fnl.h>

//////////////////////////////////////////////////////////////////////////

namespace freya { // Start of freya

///---------------------------------------------------------------------------------------------------------------------
/// NoiseGenerator
struct NoiseGenerator {
  FastNoiseLite noise;
  NoiseGeneratorDesc desc;
};
/// NoiseGenerator
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// NoiseGenerator functions

NoiseGenerator* noise_generator_create(const NoiseGeneratorDesc& desc) {
  NoiseGenerator* gen = new NoiseGenerator{};

  noise_generator_set_desc(gen, desc);
  return gen;
}

void noise_generator_destroy(NoiseGenerator* gen) {
  if(!gen) {
    return;
  }

  delete gen;
}

void noise_generator_set_desc(NoiseGenerator* gen, const NoiseGeneratorDesc& desc) {
  FREYA_DEBUG_ASSERT(gen, "");
  gen->desc = desc;

  // Setting the types 
  
  gen->noise.SetNoiseType((FastNoiseLite::NoiseType)desc.noise_type);
  gen->noise.SetRotationType3D((FastNoiseLite::RotationType3D)desc.rotation_type);
  gen->noise.SetFractalType((FastNoiseLite::FractalType)desc.fractal_type);
  
  gen->noise.SetCellularDistanceFunction((FastNoiseLite::CellularDistanceFunction)desc.distance_func);
  gen->noise.SetCellularReturnType((FastNoiseLite::CellularReturnType)desc.return_type);
  gen->noise.SetDomainWarpType((FastNoiseLite::DomainWarpType)desc.warp_type);

  // Setting the fractals 

  gen->noise.SetSeed(desc.seed);
  gen->noise.SetFrequency(desc.frequency);

  gen->noise.SetFractalOctaves(desc.octaves);
  gen->noise.SetFractalLacunarity(desc.lacunarity);
  gen->noise.SetFractalGain(desc.gain);
  gen->noise.SetFractalWeightedStrength(desc.strength);
  gen->noise.SetFractalPingPongStrength(desc.ping_pong_strength);

  // Setting the other values
  
  gen->noise.SetCellularJitter(desc.cellular_jitter);
  gen->noise.SetDomainWarpAmp(desc.domain_amp);
}

NoiseGeneratorDesc& noise_generator_get_desc(NoiseGenerator* gen) {
  FREYA_DEBUG_ASSERT(gen, "");
  return gen->desc;
}

f32 noise_generator_get(NoiseGenerator* gen, const Vec2& coords) {
  FREYA_DEBUG_ASSERT(gen, "");
  return gen->noise.GetNoise(coords.x, coords.y);
}

f32 noise_generator_get(NoiseGenerator* gen, const Vec3& coords) {
  FREYA_DEBUG_ASSERT(gen, "");
  return gen->noise.GetNoise(coords.x, coords.y, coords.z);
}

void noise_generator_domain_warp(NoiseGenerator* gen, Vec2& out_coords) {
  FREYA_DEBUG_ASSERT(gen, "");
  gen->noise.DomainWarp(out_coords.x, out_coords.y);
}

void noise_generator_domain_warp(NoiseGenerator* gen, Vec3& out_coords) {
  FREYA_DEBUG_ASSERT(gen, "");
  gen->noise.DomainWarp(out_coords.x, out_coords.y, out_coords.z);
}

/// NoiseGenerator functions
///---------------------------------------------------------------------------------------------------------------------

} // End of freya

//////////////////////////////////////////////////////////////////////////

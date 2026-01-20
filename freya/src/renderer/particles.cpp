#include "freya_render.h"
#include "freya_timer.h"

//////////////////////////////////////////////////////////////////////////

namespace freya { // Start of freya 

///---------------------------------------------------------------------------------------------------------------------
/// Private functions

static void apply_random_distribution(ParticleEmitter& emitter) {
  for(sizei i = 0; i < emitter.particles_count; i++) {
    Vec2 direction         = Vec2(random_f32(-1.0f, 1.0f), 
                                  random_f32(-1.0f, 1.0f));
    emitter.velocities[i] *= direction;
  }
}

static void apply_square_distribution(ParticleEmitter& emitter) {
  f32 min = -emitter.distribution_radius;
  f32 max = emitter.distribution_radius;

  for(sizei i = 0; i < emitter.particles_count; i++) {
    Vec2 direction         = Vec2(random_f32(min, max), 
                                  random_f32(min, max));
    emitter.velocities[i] *= direction;
  }
}

static void apply_circular_distribution(ParticleEmitter& emitter) {
  for(sizei i = 0; i < emitter.particles_count; i++) {
    f32 theta_angle = random_f32(0.0f, 2.0f * PI);
    f32 radius      = random_f32(0.0f, 1.0f) * emitter.distribution_radius;

    emitter.velocities[i] *= Vec2(freya::cos(theta_angle) * radius, freya::sin(theta_angle) * radius); 
  }
}

/// Private functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// ParticleEmitter functions

void particle_emitter_create(ParticleEmitter& out_emitter, const ParticleEmitterDesc& desc) {
  // Distribution variables init

  out_emitter.distribution_radius = desc.distribution_radius; 
  out_emitter.distribution        = desc.distribution;
  out_emitter.particles_count     = desc.count;
  
  // Positional variables init 
  
  out_emitter.initial_position = desc.position;
  out_emitter.initial_scale    = desc.scale;
  out_emitter.initial_velocity = desc.velocity;

  for(sizei i = 0; i < desc.count; i++) {
    Transform& transform = out_emitter.transforms[i];

    transform.position = desc.position; 
    transform.scale    = desc.scale;
  }

  // Physics variables init

  for(sizei i = 0; i < desc.count; i++) {
    out_emitter.forces[i] = Vec2(0.0f);
  }
  
  for(sizei i = 0; i < desc.count; i++) {
    out_emitter.velocities[i] = desc.velocity;
  }
  
  out_emitter.gravity_factor = desc.gravity_factor; 

  // Render variables init
  
  out_emitter.texture = (desc.texture_id.get_id() != ASSET_ID_INVALID) ? asset_group_get_texture(desc.texture_id) : nullptr;
  out_emitter.color   = desc.color;

  // Timer init
  timer_create(out_emitter.lifetime, desc.lifetime, false);
}

void particle_emitter_update(ParticleEmitter& emitter, const f32 delta_time) {
  if(!emitter.is_active) {
    return;
  }

  // Apply the numarical integrator for each particle 

  for(sizei i = 0; i < emitter.particles_count; i++) {
    Vec2 acceleration = emitter.forces[i] * -1.0f; // -1.0f = inverse mass
    acceleration.y   += emitter.gravity_factor;

    emitter.velocities[i]          += acceleration * delta_time;
    emitter.transforms[i].position += emitter.velocities[i] * delta_time; 

    emitter.forces[i] = Vec2(0.0f);
  }

  // Update the timer 

  timer_update(emitter.lifetime, delta_time);
  if(!emitter.lifetime.has_runout) {
    return;
  }

  // Bye bye, emitter. Goodnight
  emitter.is_active = false; 
}

void particle_emitter_emit(ParticleEmitter& emitter) {
  particle_emitter_reset(emitter);
  emitter.is_active = true;

  // Applying the distribution

  switch(emitter.distribution) {
    case DISTRIBUTION_RANDOM: 
      apply_random_distribution(emitter);
      break;
    case DISTRIBUTION_SQUARE: 
      apply_square_distribution(emitter);
      break;
    case DISTRIBUTION_CIRCULAR: 
      apply_circular_distribution(emitter);
      break;
    default:
      break;
  }
}

void particle_emitter_reset(ParticleEmitter& emitter) {
  emitter.is_active = false;
  timer_reset(emitter.lifetime);
  
  for(sizei i = 0; i < emitter.particles_count; i++) {
    emitter.transforms[i].position = emitter.initial_position;
    emitter.transforms[i].scale    = emitter.initial_scale;
  }
  
  for(sizei i = 0; i < emitter.particles_count; i++) {
    emitter.forces[i] = Vec2(0.0f);
  }
  
  for(sizei i = 0; i < emitter.particles_count; i++) {
    emitter.velocities[i] = emitter.initial_velocity;
  }
}

/// ParticleEmitter functions
///---------------------------------------------------------------------------------------------------------------------

} // End of freya

//////////////////////////////////////////////////////////////////////////

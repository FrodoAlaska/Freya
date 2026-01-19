#include "freya_render.h"
#include "freya_timer.h"

//////////////////////////////////////////////////////////////////////////

namespace freya { // Start of freya 

///---------------------------------------------------------------------------------------------------------------------
/// Private functions

static void apply_normal_distribution(ParticleEmitter& emitter) {
  for(sizei i = 0; i < emitter.particles_count; i++) {
    Vec2 direction         = Vec2(random_f32(-emitter.distribution_radius, emitter.distribution_radius), 
                                  random_f32(-emitter.distribution_radius, emitter.distribution_radius));
    emitter.velocities[i] *= direction;
  }
}

static void apply_square_distribution(ParticleEmitter& emitter) {
  Vec2 min = (emitter.initial_position - (emitter.distribution_radius / 2.0f));
  Vec2 max = min + emitter.distribution_radius;

  min = vec2_normalize(min);
  max = vec2_normalize(max);

  for(sizei i = 0; i < emitter.particles_count; i++) {
    Vec2 direction         = Vec2(random_f32(min.x, max.x), 1.0f);
    emitter.velocities[i] *= direction;
  }
}

/// Private functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// ParticleEmitter functions

void particle_emitter_create(ParticleEmitter& out_emitter, const ParticleEmitterDesc& desc) {
  // Setting default values 
  
  out_emitter.initial_position = desc.position;
  out_emitter.initial_velocity = desc.velocity;

  out_emitter.particles_count  = desc.count;
  out_emitter.gravity_factor   = desc.gravity_factor; 

  out_emitter.distribution_radius = desc.distribution_radius; 
  out_emitter.distribution        = desc.distribution;

  for(sizei i = 0; i < desc.count; i++) {
    Transform& transform = out_emitter.transforms[i];

    transform.position = desc.position; 
    transform.scale    = desc.scale;
    transform_apply(transform);
  }

  for(sizei i = 0; i < desc.count; i++) {
    out_emitter.forces[i] = Vec3(0.0f);
  }
  
  for(sizei i = 0; i < desc.count; i++) {
    out_emitter.velocities[i] = desc.velocity;
  }

  // Setting render variables 
  
  out_emitter.texture = (desc.texture_id.get_id() != ASSET_ID_INVALID) ? asset_group_get_texture(desc.texture_id) : nullptr;
  out_emitter.color   = desc.color;

  // Create the timer 
  timer_create(out_emitter.lifetime, desc.lifetime, false);
}

void particle_emitter_update(ParticleEmitter& emitter, const f32 delta_time) {
  if(!emitter.is_active) {
    return;
  }

  // Apply the numarical integrator for each particle 

  for(sizei i = 0; i < emitter.particles_count; i++) {
    emitter.transforms[i].position += (emitter.velocities[i] + Vec2(0.0f, emitter.gravity_factor)) * delta_time; 
    transform_translate(emitter.transforms[i], emitter.transforms[i].position);
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
  if(emitter.is_active) {
    particle_emitter_reset(emitter);
  }

  emitter.is_active = true;

  // Applying the distribution

  switch(emitter.distribution) {
    case DISTRIBUTION_RANDOM: 
      apply_normal_distribution(emitter);
      break;
    case DISTRIBUTION_SQUARE: 
      apply_square_distribution(emitter);
      break;
    default:
      break;
  }
}

void particle_emitter_reset(ParticleEmitter& emitter) {
  emitter.is_active = false;
  timer_reset(emitter.lifetime);
  
  for(sizei i = 0; i < emitter.particles_count; i++) {
    transform_translate(emitter.transforms[i], emitter.initial_position);
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

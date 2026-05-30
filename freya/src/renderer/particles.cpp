#include "freya_render.h"
#include "freya_timer.h"

//////////////////////////////////////////////////////////////////////////

namespace freya { // Start of freya 

///---------------------------------------------------------------------------------------------------------------------
/// Private functions

static void apply_random_distribution(ParticleEmitter& emitter) {
  for(i32 i = 0; i < emitter.particles_count; i++) {
    Vec2 direction         = Vec2(random_f32(-1.0f, 1.0f), 
                                  random_f32(-1.0f, 1.0f));
    emitter.velocities[i] *= direction;
  }
}

static void apply_square_distribution(ParticleEmitter& emitter) {
  f32 min = -emitter.distribution_radius;
  f32 max = emitter.distribution_radius;

  for(i32 i = 0; i < emitter.particles_count; i++) {
    Vec2 direction         = Vec2(random_f32(min, max), 
                                  random_f32(min, max));
    emitter.velocities[i] *= direction;
  }
}

static void apply_circular_distribution(ParticleEmitter& emitter) {
  for(i32 i = 0; i < emitter.particles_count; i++) {
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
  
  out_emitter.initial_scale    = desc.scale;
  out_emitter.initial_velocity = desc.velocity;

  for(sizei i = 0; i < desc.count; i++) {
    Transform& transform = out_emitter.transforms[i];

    transform.position = Vec2(-1000.0f); 
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
 
  if(desc.texture_id.get_id() != ASSET_ID_INVALID) {
    out_emitter.texture = asset_group_get_texture(desc.texture_id);
  }

  out_emitter.color   = desc.color;

  // Timer init
  timer_create(out_emitter.lifetime, desc.lifetime, false);
}

void particle_emitter_create(ParticleEmitter& out_emitter, const AssetID& config_id) {
  // Get the LUA state 
  lua_State* lua = asset_group_get_lua_state(config_id);

  //
  // Fill the desc using the LUA config
  //

  ParticleEmitterDesc desc;
  lua_getglobal(lua, "particle");

  // Velocity
  
  i32 type = lua_getfield(lua, -1, "velocity");
  if(type != LUA_TNIL) {
    lua_geti(lua, -1, 1);
    desc.velocity.x = lua_tonumber(lua, -1); 
    lua_pop(lua, 1); 
    
    lua_geti(lua, -1, 2);
    desc.velocity.y = lua_tonumber(lua, -1); 
    lua_pop(lua, 1); 

    lua_pop(lua, 1);
  } 

  // Count

  type = lua_getfield(lua, -1, "count");
  if(type != LUA_TNIL) {
    desc.count = lua_tonumber(lua, -1);
    lua_pop(lua, 1);
  }

  // Scale
  
  type = lua_getfield(lua, -1, "scale");
  if(type != LUA_TNIL) {
    lua_geti(lua, -1, 1);
    desc.scale.x = lua_tonumber(lua, -1); 
    lua_pop(lua, 1); 
    
    lua_geti(lua, -1, 2);
    desc.scale.y = lua_tonumber(lua, -1); 
    lua_pop(lua, 1); 

    lua_pop(lua, 1);
  }

  // Color
  
  type = lua_getfield(lua, -1, "color");
  if(type != LUA_TNIL) {
    lua_geti(lua, -1, 1);
    desc.color.r = lua_tonumber(lua, -1); 
    lua_pop(lua, 1); 
    
    lua_geti(lua, -1, 2);
    desc.color.g = lua_tonumber(lua, -1); 
    lua_pop(lua, 1); 
    
    lua_geti(lua, -1, 3);
    desc.color.b = lua_tonumber(lua, -1); 
    lua_pop(lua, 1); 
    
    lua_geti(lua, -1, 4);
    desc.color.a = lua_tonumber(lua, -1); 
    lua_pop(lua, 1); 

    lua_pop(lua, 1);
  }

  // Lifetime

  type = lua_getfield(lua, -1, "lifetime");
  if(type != LUA_TNIL) {
    desc.lifetime = lua_tonumber(lua, -1);
    lua_pop(lua, 1);
  }
  
  // Gravity factor

  type = lua_getfield(lua, -1, "gravity_factor");
  if(type != LUA_TNIL) {
    desc.gravity_factor = lua_tonumber(lua, -1);
    lua_pop(lua, 1);
  }
  
  // Distribution

  type = lua_getfield(lua, -1, "distribution");
  if(type != LUA_TNIL) {
    // @NOTE: Yes, string comparisons. Leave me alone.
    
    String type_str = lua_tostring(lua, -1);

    if(type_str == "random") {
      desc.distribution = DISTRIBUTION_RANDOM;
    } 
    else if(type_str == "square") {
      desc.distribution = DISTRIBUTION_SQUARE;
    }
    else if(type_str == "circular") {
      desc.distribution = DISTRIBUTION_CIRCULAR;
    }

    lua_pop(lua, 1);
  }
  
  // Radius

  type = lua_getfield(lua, -1, "radius");
  if(type != LUA_TNIL) {
    desc.distribution_radius = lua_tonumber(lua, -1);
    lua_pop(lua, 1);
  }

  // Done!
  lua_pop(lua, 1);

  // Create the emitter
  particle_emitter_create(out_emitter, desc);
}

void particle_emitter_update(ParticleEmitter& emitter, const f32 delta_time) {
  if(!emitter.is_active) {
    return;
  }

  // Apply the numarical integrator for each particle 

  for(i32 i = 0; i < emitter.particles_count; i++) {
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

void particle_emitter_emit(ParticleEmitter& emitter, const Vec2& position) {
  particle_emitter_reset(emitter, position);
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

void particle_emitter_reset(ParticleEmitter& emitter, const Vec2& position) {
  emitter.is_active = false;
  timer_reset(emitter.lifetime);
  
  for(i32 i = 0; i < emitter.particles_count; i++) {
    emitter.transforms[i].position = position;
    emitter.transforms[i].scale    = emitter.initial_scale;
  }
  
  for(i32 i = 0; i < emitter.particles_count; i++) {
    emitter.forces[i] = Vec2(0.0f);
  }
  
  for(i32 i = 0; i < emitter.particles_count; i++) {
    emitter.velocities[i] = emitter.initial_velocity;
  }
}

/// ParticleEmitter functions
///---------------------------------------------------------------------------------------------------------------------

} // End of freya

//////////////////////////////////////////////////////////////////////////

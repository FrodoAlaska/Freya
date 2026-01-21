#include "freya_math.h"

//////////////////////////////////////////////////////////////////////////

namespace freya { // Start of freya

/// ----------------------------------------------------------------------
/// Transform functions

void transform_lerp(Transform& trans_a, const Transform& trans_b, const f32 delta) {
  trans_a.position = vec2_lerp(trans_a.position, trans_b.position, delta);
  trans_a.rotation = lerp(trans_a.rotation, trans_b.rotation, delta);
  trans_a.scale    = vec2_lerp(trans_a.scale, trans_b.scale, delta);
}

void transform_lerp(Transform& trans, 
                    const Vec3& position, 
                    const f32 rotation, 
                    const Vec3& scale,
                    const f32 delta) {
  trans.position = vec2_lerp(trans.position, position, delta);
  trans.rotation = lerp(trans.rotation, rotation, delta);
  trans.scale    = vec2_lerp(trans.scale, scale, delta);
}

void transform_lerp_position(Transform& trans, const Vec2& position, const f32 delta) {
  trans.position = vec2_lerp(trans.position, position, delta);
}

void transform_lerp_rotation(Transform& trans, const f32 rotation, const f32 delta) {
  trans.rotation = lerp(trans.rotation, rotation, delta);
}

void transform_lerp_scale(Transform& trans, const Vec2& scale, const f32 delta) {
  trans.scale = vec2_lerp(trans.scale, scale, delta);
}

/// Transform functions
/// ----------------------------------------------------------------------

} // End of freya

//////////////////////////////////////////////////////////////////////////

#pragma once

#include "freya_base.h"
#include "freya_containers.h"

#include "freya_pch.h"

//////////////////////////////////////////////////////////////////////////

namespace freya { // Start of freya

///---------------------------------------------------------------------------------------------------------------------
/// Math consts 

/// The value of pi
const f64 PI        = 3.14159265359;

/// A multiplier to convert a value from radians to degrees
const f32 RAD2DEG   = (180.0f / PI);

/// A multiplier to convert a value from degrees to radians
const f32 DEG2RAD   = (PI / 180.0f);

/// The value of epsilon
const f64 EPSILON   = 1.192092896e-07f;

/// The minimum possible float value
const f64 FLOAT_MIN = -3.40282e+38F;

/// The maximum possible float value
const f64 FLOAT_MAX = 3.40282e+38F;

/// Math consts 
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Macros

/// Convert the given `x` to a value representing a Kibibyte.
#define KiB(x)        (x * 1024)

/// Convert the given `x` to a value representing a Mebibyte.
#define MiB(x)        ((KiB(x)) * 1024)

/// Convert the given `x` to a value representing a Gibibyte.
#define GiB(x)        ((MiB(x)) * 1024)

/// Convert the given `x` to radians
#define FREYA_TO_RADIANS(x) (x * freya::DEG2RAD)

/// Convert the given `x` to degrees
#define FREYA_TO_DEGREES(x) (x * freya::RAD2DEG)

/// Macros
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Vec2
typedef glm::vec2 Vec2;
/// Vec2
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// IVec2
typedef glm::ivec2 IVec2;
/// IVec2
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// BVec2
typedef glm::bvec2 BVec2;
/// BVec2
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Vec3
typedef glm::vec3 Vec3;
/// Vec3
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// IVec3
typedef glm::ivec3 IVec3;
/// IVec3
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// BVec3
typedef glm::bvec3 BVec3;
/// BVec3
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Vec4
typedef glm::vec4 Vec4;
/// Vec4
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// IVec4
typedef glm::ivec4 IVec4;
/// IVec4
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// BVec4
typedef glm::bvec4 BVec4;
/// BVec4
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Mat3
typedef glm::mat3 Mat3;
/// Mat3
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Mat4
typedef glm::mat4 Mat4;
/// Mat4
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Mat4x3
typedef glm::mat4x3 Mat4x3;
/// Mat4x3
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Quat
typedef glm::quat Quat;
/// Quat
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Transform
struct Transform {
  Vec2 position = Vec2(0.0f); 
  Vec2 scale    = Vec2(1.0f);
  f32 rotation  = 0.0f;
};
/// Transform
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Vertex2D
struct Vertex2D {
  Vec2 position; 
  Vec2 normal;
  Vec2 texture_coords;
  Vec4 color;
};
/// Vertex3D
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Rect2D
struct Rect2D {
  Vec2 size; 
  Vec2 position;
};
/// Rect2D
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Math common functions

/// Clamp the float `value` between `min` and `max`
FREYA_API const f32 clamp_float(const f32 value, const f32 min, const f32 max);

/// Clamp the int `value` between `min` and `max`
FREYA_API const i32 clamp_int(const i32 value, const i32 min, const i32 max);

/// Returns the cosine of `x`
FREYA_API const f64 cos(const f64 x);

/// Returns the sine of `x`
FREYA_API const f64 sin(const f64 x);

/// Returns the tangent of `x`
FREYA_API const f64 tan(const f64 x);

/// Returns the tangent of `x`
FREYA_API const f64 atan(const f64 x);

/// Returns the tangent of `y` and `x`
FREYA_API const f64 atan(const f64 y, const f64 x);

/// Returns the floored value of `x`
FREYA_API const f64 floor(const f64 x);

/// Returns the square root of `x`
FREYA_API const f64 sqrt(const f64 x);

/// Returns the absolute value `x`
FREYA_API const f32 abs(const f32 x);

/// Returns `x` raised to the power of y
FREYA_API const f32 pow(const f32 x, const f32 y);

/// Returns `x - y * floor(x / y)`.
FREYA_API const f32 fmod(const f32 x, const f32 y);

/// Returns the lesser value between `x` and `y`
FREYA_API const f32 min_float(const f32 x, const f32 y);

/// Returns the greater value between `x` and `y`
FREYA_API const f32 max_float(const f32 x, const f32 y);

/// Returns the lesser value between `x` and `y`
FREYA_API const i32 min_int(const i32 x, const i32 y);

/// Returns the greater value between `x` and `y`
FREYA_API const i32 max_int(const i32 x, const i32 y);

/// Returns the linear interpolation from `start` to `end` by `amount`
FREYA_API const f32 lerp(const f32 start, const f32 end, const f32 amount);

/// Perform a hermite interpolation between `edge0` and `edge1` by `x`.
FREYA_API const f32 smoothstep(const f32 edge0, const f32 edge1, const f32 x);

/// Returns the re mapped `value` from `old_min` - `old_max` to `new_min` to `new_max`
FREYA_API const f32 remap(const f32 value, const f32 old_min, const f32 old_max, const f32 new_min, const f32 new_max);

/// Math common functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Math random functions

/// Returns a random 32-bit float value
FREYA_API const f32 random_f32();

/// Returns a random 32-bit float value between `min` and `max`
FREYA_API const f32 random_f32(const f32 min, const f32 max);

/// Returns a random 64-bit float value
FREYA_API const f64 random_f64();

/// Returns a random 64-bit float value between `min` and `max`
FREYA_API const f64 random_f64(const f64 min, const f64 max);

/// Returns a random 32-bit signed int value
FREYA_API const i32 random_i32();

/// Returns a random 32-bit signed int value between `min` and `max`
FREYA_API const i32 random_i32(const i32 min, const i32 max);

/// Returns a random 64-bit signed int value
FREYA_API const i64 random_i64();

/// Returns a random 64-bit signed int value between `min` and `max`
FREYA_API const i64 random_i64(const i64 min, const i64 max);

/// Returns a random 32-bit unsigned int value
FREYA_API const u32 random_u32();

/// Returns a random 32-bit unsigned int value between `min` and `max`
FREYA_API const u32 random_u32(const u32 min, const u32 max);

/// Returns a random 64-bit unsigned int value
FREYA_API const u64 random_u64();

/// Returns a random 64-bit unsigned int value between `min` and `max`
FREYA_API const u64 random_u64(const u64 min, const u64 max);

/// Math random functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Vec2 functions

/// Returns the normalized vector of `v`
FREYA_API const Vec2 vec2_normalize(const Vec2& v);

/// Returns the clamped `value` between `min` and `max`
FREYA_API const Vec2 vec2_clamp(const Vec2& value, const Vec2& min, const Vec2& max);

/// Returns the linearly interpolated vector from `start` to `end` by `amount`.
FREYA_API const Vec2 vec2_lerp(const Vec2& start, const Vec2& end, const f32 amount);

/// Perform a hermite interpolation between `edge0` and `edge1` by `x`.
FREYA_API const Vec2 vec2_smoothstep(const Vec2& edge0, const Vec2& edge1, const Vec2& x);

/// Returns the lesser vector between `v1` and `v2`
FREYA_API const Vec2 vec2_min(const Vec2& v1, const Vec2& v2);

/// Returns the greater vector between `v1` and `v2`
FREYA_API const Vec2 vec2_max(const Vec2& v1, const Vec2& v2);

/// Returns the dot product between `v1` and `v2`
FREYA_API const f32 vec2_dot(const Vec2& v1, const Vec2& v2);

/// Returns the distance between `v1` and `v2`
FREYA_API const f32 vec2_distance(const Vec2& vec, const Vec2& target);

/// Returns the angle in radians between `point1` and `point2`
FREYA_API const f32 vec2_angle(const Vec2& point1, const Vec2& point2);

/// Returns the string representation of `vec`
FREYA_API const String vec2_to_string(const Vec2& vec);

/// Vec2 functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Vec3 functions

/// Returns the normalized vector of `v`
FREYA_API const Vec3 vec3_normalize(const Vec3& v);

/// Returns the clamped `value` between `min` and `max`
FREYA_API const Vec3 vec3_clamp(const Vec3& value, const Vec3& min, const Vec3& max);

/// Returns the linearly interpolated vector from `start` to `end` by `amount`.
FREYA_API const Vec3 vec3_lerp(const Vec3& start, const Vec3& end, const f32 amount);

/// Perform a hermite interpolation between `edge0` and `edge1` by `x`.
FREYA_API const Vec3 vec3_smoothstep(const Vec3& edge0, const Vec3& edge1, const Vec3& x);

/// Returns the lesser vector between `v1` and `v2`
FREYA_API const Vec3 vec3_min(const Vec3& v1, const Vec3& v2);

/// Returns the greater vector between `v1` and `v2`
FREYA_API const Vec3 vec3_max(const Vec3& v1, const Vec3& v2);

/// Returns the dot product between `v1` and `v2`
FREYA_API const f32 vec3_dot(const Vec3& v1, const Vec3& v2);

/// Returns the cross product between `v1` and `v2`
FREYA_API const Vec3 vec3_cross(const Vec3& v1, const Vec3& v2);

/// Returns the distance between `point1` and `point2`
FREYA_API const f32 vec3_distance(const Vec3& v1, const Vec3& v2);

/// Returns the string representation of `vec`
FREYA_API const String vec3_to_string(const Vec3& vec);

/// Vec3 functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Vec4 functions

/// Returns the normalized vector of `v`
FREYA_API const Vec4 vec4_normalize(const Vec4& v);

/// Returns the clamped `value` between `min` and `max`
FREYA_API const Vec4 vec4_clamp(const Vec4& value, const Vec4& min, const Vec4& max);

/// Returns the linearly interpolated vector from `start` to `end` by `amount`.
FREYA_API const Vec4 vec4_lerp(const Vec4& start, const Vec4& end, const f32 amount);

/// Perform a hermite interpolation between `edge0` and `edge1` by `x`.
FREYA_API const Vec4 vec4_smoothstep(const Vec4& edge0, const Vec4& edge1, const Vec4& x);

/// Returns the lesser vector between `v1` and `v2`
FREYA_API const Vec4 vec4_min(const Vec4& v1, const Vec4& v2);

/// Returns the greater vector between `v1` and `v2`
FREYA_API const Vec4 vec4_max(const Vec4& v1, const Vec4& v2);

/// Returns the dot product between `v1` and `v2`
FREYA_API const f32 vec4_dot(const Vec4& v1, const Vec4& v2);

/// Returns the distance between `point1` and `point2`
FREYA_API const f32 vec4_distance(const Vec4& v1, const Vec4& v2);

/// Returns the string representation of `vec`
FREYA_API const String vec4_to_string(const Vec4& vec);

/// Vec4 functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Mat3 functions

/// Returns the determinant value of `mat` 
FREYA_API const f32 mat3_det(const Mat3& mat);

/// Returns the transposed matrix of `mat` 
FREYA_API const Mat3 mat3_transpose(const Mat3& mat);

/// Returns the inverse matrix of `mat` 
FREYA_API const Mat3 mat3_inverse(const Mat3& mat);

/// Returns the raw array of floats of `mat` 
FREYA_API const f32* mat3_raw_data(const Mat3& mat);

/// Returns the string representation of `mat3`
FREYA_API const String mat3_to_string(const Mat3& mat);

/// Mat3 functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Mat4 functions

/// Creates and returns a `Mat4` from the given array 
/// of `values`. 
///
/// @NOTE: The `values` array MUST have 16 elements.
FREYA_API Mat4 mat4_make(const f32* values);

/// Returns the determinant value of `mat` 
FREYA_API const f32 mat4_det(const Mat4& mat);

/// Returns the transposed matrix of `mat` 
FREYA_API const Mat4 mat4_transpose(const Mat4& mat);

/// Returns the inverse matrix of `mat` 
FREYA_API const Mat4 mat4_inverse(const Mat4& mat);

/// Returns the translation matrix affected by `position`
FREYA_API const Mat4 mat4_translate(const Vec3& position);

/// Returns the roation matrix affected by `axis` and `angle`
FREYA_API const Mat4 mat4_rotate(const Vec3& axis, const f32 angle);

/// Returns the sclae matrix affected by `scale`
FREYA_API const Mat4 mat4_scale(const Vec3& scale);

/// Return a matrix that has been translated, rotated, 
/// and scaled according to `transform`.
FREYA_API const Mat4 mat4_transform(const Transform& transform);

/// Generates a 4x4 matrix for a symmetric perspective view frustrum, using the left hand coordinate system.
///
/// @NOTE: The fov is in radians.
FREYA_API const Mat4 mat4_perspective(const f32 fov, const f32 aspect_ratio, const f32 near, const f32 far);

/// Generates a 4x4 matrix for a orthographic view frustrum for a 2D space, 
/// using the left hand coordinate system
FREYA_API const Mat4 mat4_ortho(const f32 left, const f32 right, 
                                 const f32 bottom, const f32 top);

/// Generates a 4x4 matrix for a orthographic view frustrum for a 3D space, 
/// using the left hand coordinate system
FREYA_API const Mat4 mat4_ortho(const f32 left, const f32 right, 
                                 const f32 bottom, const f32 top, 
                                 const f32 near, const f32 far);

/// Generates a 4x4 look at view matrix, using ther left handed coordinate system 
FREYA_API const Mat4 mat4_look_at(const Vec3& eye, const Vec3& center, const Vec3& up);

/// Returns the raw array of floats of `mat` 
FREYA_API const f32* mat4_raw_data(const Mat4& mat);

/// Returns the string representation of `mat3`
FREYA_API const String mat4_to_string(const Mat4& mat);

/// Mat4 functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Math Quat functions

/// Create and return an identity quaternion.
FREYA_API Quat quat_identity();

/// Returns the dot product between `q1` and `q2`.
FREYA_API const f32 quat_dot(const Quat& q1, const Quat& q2);

/// Returns the normalized quaternion of `q`.
FREYA_API const Quat quat_normalize(const Quat& q);

/// Returns the linearly interpolated quaternion from `start` to `end` by `amount`.
FREYA_API const Quat quat_lerp(const Quat& start, const Quat& end, const f32 amount);

/// Returns the smooth linearly interpolated quaternion from `start` to `end` by `amount`.
FREYA_API const Quat quat_slerp(const Quat& start, const Quat& end, const f32 amount);

/// Sets and returns the rotation of a quaternion using the given 3x3 `mat`.
FREYA_API const Quat quat_set_mat3(const Mat3& mat);

/// Sets and returns the rotation of a quaternion using the given 4x4 `mat`.
FREYA_API const Quat quat_set_mat4(const Mat4& mat);

/// Rotates and returns a quaternion using the given `axis` by `angle` in radians.
FREYA_API const Quat quat_angle_axis(const Vec3& axis, const f32 angle);

/// Convert the given `q` to a 3x3 rotation matrix
FREYA_API const Mat3 quat_to_mat3(const Quat& q);

/// Convert the given `q` to a 4x4 rotation matrix
FREYA_API const Mat4 quat_to_mat4(const Quat& q);

/// Convert the given `q` to a `Vec3` representing an euler angle. 
/// The resulting angles will be expressed in radians.
FREYA_API const Vec3 quat_to_euler(const Quat& q);

/// Math Quat functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Transform functions

/// Lerp between the two given transforms `trans_a` and `trans_b` by the given `delta` value.
FREYA_API void transform_lerp(Transform& trans_a, const Transform& trans_b, const f32 delta);

/// Lerp the components of the given transform `trans` to `position`, `rotation`, 
/// and `scale` by the given `delta` value.
FREYA_API void transform_lerp(Transform& trans, 
                               const Vec2& position, 
                               const f32 rotation, 
                               const Vec2& scale,
                               const f32 delta);

/// Lerp the position component of the given transform `trans` to `position` by the given `delta` value.
FREYA_API void transform_lerp_position(Transform& trans, const Vec2& position, const f32 delta);

/// Use the linear interpolation method to interpolate the roation component 
/// of `trans` to `rotation` by `delta`.
FREYA_API void transform_lerp_rotation(Transform& trans, const f32 rotation, const f32 delta);

/// Lerp the scale component of the given transform `trans` to `scale` by the given `delta` value.
FREYA_API void transform_lerp_scale(Transform& trans, const Vec2& scale, const f32 delta);

/// Transform functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Point functions

/// Check if the given `point` is inside the bounds of `position` and `position + size`.
FREYA_API bool point_in_rect(const Vec2& point, const Vec2& position, const Vec2& size);

/// Check if the given `point` is inside the bounds `rect`.
FREYA_API bool point_in_rect(const Vec2& point, const Rect2D& rect);

/// Check if the given `point` is inside the circle centered at `position` with `radius` radius.
FREYA_API bool point_in_circle(const Vec2& point, const Vec2& position, const f32 radius);

/// Point functions
///---------------------------------------------------------------------------------------------------------------------

} // End of freya

//////////////////////////////////////////////////////////////////////////

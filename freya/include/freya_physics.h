#pragma once

#include "freya_math.h"

#include <box2d/id.h> // @TODO (Physics): NOOOOOOO!!!!!!

//////////////////////////////////////////////////////////////////////////

namespace freya { // Start of freya

///---------------------------------------------------------------------------------------------------------------------
/// Consts

/// The default conversion between meters to pixels. 
const f32 PHYSICS_METERS_TO_PIXELS = 50.0f;

/// The default conversion between pixels to meters. 
const f32 PHYSICS_PIXELS_TO_METERS = 0.02f;

/// Consts
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// PhysicsObjectLayer
enum PhysicsObjectLayer {
  /// This flag indicates a "none" layer, 
  /// which can be used to disable collisions 
  /// with certain layers in the layers matrix.
  PHYSICS_OBJECT_LAYER_NONE = 0x00,
  
  /// The rest of the available layers, with 
  /// the starting layer being set to `0x01`.
  
  PHYSICS_OBJECT_LAYER_0 = 0x01,
  PHYSICS_OBJECT_LAYER_1 = 0x02,

  PHYSICS_OBJECT_LAYER_2 = 0x03,
  PHYSICS_OBJECT_LAYER_3 = 0x04,
  
  PHYSICS_OBJECT_LAYER_4 = 0x05,
  PHYSICS_OBJECT_LAYER_5 = 0x06,

  /// The maximum number of layers that 
  /// currently can be used.
  PHYSICS_OBJECT_LAYERS_MAX = 6,
};
/// PhysicsObjectLayer
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// PhysicsBodyType
enum PhysicsBodyType {
  /// Indicates to a physics body to be static, 
  /// completely unmoving, even by external forces.
  PHYSICS_BODY_STATIC, 

  /// Indicates to a physics body to be dynmaic. 
  /// Affected by all forces. 
  PHYSICS_BODY_DYNAMIC, 

  /// Indicates to a physics body to be kinematic. 
  /// Not affected by external forces, but movable.
  PHYSICS_BODY_KINEMATIC, 
};
/// PhysicsBodyType
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// ColliderType
enum ColliderType {
  /// Based to the `collider_create` function 
  /// to construct a box collider.
  COLLIDER_BOX = 0, 
  
  /// Based to the `collider_create` function 
  /// to construct a sphere collider.
  COLLIDER_CIRCLE, 
  
  /// Based to the `collider_create` function 
  /// to construct a capsule collider.
  COLLIDER_CAPSULE,
};
/// ColliderType
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// PhysicsBody
using PhysicsBodyID = b2BodyId;
/// PhysicsBody
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Collider
using ColliderID   = b2ShapeId;
/// Collider
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// CollisionData
struct CollisionData {
  /// The physics body that were involved 
  /// in the collision.
  
  PhysicsBodyID body1;
  PhysicsBodyID body2; 

  /// The normal that was generated from the 
  /// collision event.
  Vec2 normal;
};
/// CollisionData
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// SensorCollisionData
struct SensorCollisionData {
  /// The body of the sensor. 
  PhysicsBodyID sensor_body;

  /// The body that was collided 
  /// with the sensor.
  PhysicsBodyID visitor_body;
};
/// SensorCollisionData
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// RayCastDesc
struct RayCastDesc {
  /// The starting point of the ray.
  Vec2 origin    = Vec2(0.0f); 

  /// The direction which the ray will be pointing towards.
  Vec2 direction = Vec2(0.0f);

  /// The distance of the ray towards `direction`.
  ///
  /// @NOTE: If you wanted to simulate an "infinite" ray, 
  /// simply make this number large, i.e `100000000.0f`.
  f32 distance   = 1.0f;

  /// The layer that the ray will be generated in.
  ///
  /// @NOTE: By default, this value is set to `PHYSICS_OBJECT_LAYER_0`.
  PhysicsObjectLayer layer       = PHYSICS_OBJECT_LAYER_0;

  /// The ray will only collide with bodies in those layers.
  ///
  /// @NOTE: By default, this value is set to `PHYSICS_OBJECT_LAYER_0 | PHYSICS_OBJECT_LAYER_1`.
  PhysicsObjectLayer mask_layers = (PhysicsObjectLayer)(PHYSICS_OBJECT_LAYER_0 | PHYSICS_OBJECT_LAYER_1);
};
/// RayCastDesc
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// RayCastResult
struct RayCastResult {
  /// The body that was hit by the ray.
  PhysicsBodyID body;

  /// The exact point in world space of the hit point.
  Vec2 point   = Vec2(0.0f);

  /// The normal vector at the point of intersection.
  Vec2 normal  = Vec2(0.0f);

  /// The fraction along ray at the point of intersection.
  f32 fraction = 0.0f;
};
/// RayCastResult
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// PhysicsBodyDesc
struct PhysicsBodyDesc {
  /// The starting position of the physics body.
  Vec2 position;
  
  /// The starting body type of the physics body.
  PhysicsBodyType type;
  
  /// The starting rotation of the physics body (in radians).
  ///
  /// @NOTE: By default it is `0.0f`.
  f32 rotation         = 0.0f;

  /// The starting linear velocity of the body.
  ///
  /// @NOTE: By default it is `Vec2(0.0f)`.
  Vec2 linear_velocity = Vec2(0.0f);

  /// The starting angular velocity of the body.
  ///
  /// @NOTE: By default it is `0.0f`.
  f32 angular_velocity = 0.0f;
 
  /// The damping applied to the linear velcity.
  ///
  /// @NOTE: By default it is `0.0f`.
  f32 linear_damping  = 0.0f;
 
  /// The damping applied to the angular velcity.
  ///
  /// @NOTE: By default it is `0.0f`.
  f32 angular_damping = 0.0f;

  /// The starting factor which will determine by 
  /// how much the physics body is affected by gravity. 
  /// Make sure to set this value between a range of `[0, 1]`. 
  ///
  /// @NOTE: This value is set to `1.0f` by default.
  f32 gravity_factor  = 1.0f;

  /// User-specific data to keep in the physics body 
  /// for later retrieval.
  void* user_data     = nullptr;

  /// Indicates whether the body should be  
  /// initially active/awake or not.
  ///
  /// @NOTE: By default this is `true`.
  bool is_active      = true;

  /// Indicates whether the rotation should be fixed 
  /// (i.e, non-rotating), which is useful for characters. 
  ///
  /// @NOTE: By default this is `false`.
  bool rotation_fixed = false;
};
/// PhysicsBodyDesc
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// ColliderDesc
struct ColliderDesc {
  /// The density of the collider, which is usually in kg/m^2.
  ///
  /// @NOTE: This value is set to `0.0f` by default.
  f32 density                    = 0.0f;
  
  /// The starting friction of the physics body.
  /// Make sure to set this value between a range of `[0, 1]`. 
  ///
  /// @NOTE: This value is set to `0.0f` by default.
  f32 friction                   = 0.0f;
  
  /// The starting restitution of the physics body.
  /// Make sure to set this value between a range of `[0, 1]`. 
  ///
  /// @NOTE: This value is set to `0.1f` by default.
  f32 restitution                = 0.0f;
  
  /// The object layer which this shape will live in.
  ///
  /// @NOTE: This value is set to `PHYSICS_OBJECT_LAYER_0` by default.
  PhysicsObjectLayer layer       = PHYSICS_OBJECT_LAYER_0;

  /// The object layers this shape is allowed to collide with.
  ///
  /// @NOTE: This value is set to `PHYSICS_OBJECT_LAYER_0 | PHYSICS_OBJECT_LAYER_1` by default.
  PhysicsObjectLayer mask_layers = (PhysicsObjectLayer)(PHYSICS_OBJECT_LAYER_0 | PHYSICS_OBJECT_LAYER_1);

  /// If this flag is set to `true`, the body will 
  /// act as a sensor, which will detect collisions 
  /// but never resolve them. Effectively, using it 
  /// as a trigger.
  ///
  /// @NOTE: By default, this value is set to `false`.
  bool is_sensor                 = false;
};
/// ColliderDesc
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Physics world functions

/// Initialize the physics world with the given `gravity`.
FREYA_API void physics_world_init(const Vec2& gravity);

/// Shutdown the physics world, reclaiming any allocated memory in the process.
FREYA_API void physics_world_shutdown();

/// Step through the physics simulation every frame, using the given `delta_time` and `sub_steps`.
///
/// The `sub_steps` parametar determines the number of iterations to go through a "physics step", 
/// which consists of collision detection followed by numerical integration.
/// The higher the value, the more accurate the physics is, but the more performance will suffer.
///
/// @NOTE: The `collision_steps` parametar is set to `4` by default.
FREYA_API void physics_world_step(const f32 delta_time, const i32 sub_steps = 4);

/// Cast a ray using the information provided by `cast_desc` into the world, 
/// firing the `EVENT_PHYSICS_RAYCAST_HIT` event upon any successful intersections.
FREYA_API void physics_world_cast_ray(const RayCastDesc& cast_desc);

/// Set the gravity of the physics world to the given `gravity`.
FREYA_API void physics_world_set_gravity(const Vec2& gravity);

/// Retrieve the current gravity value of the physics world.
FREYA_API Vec2 physics_world_get_gravity();

/// Toggle "pause" mode either on or off, depending on the current state. 
/// Pause mode will simply stop updating the physics world, which prompts 
/// all the bodies to stop simulating physics.
FREYA_API void physics_world_toggle_paused();

/// Retrieve the current pause state of the physics world.
FREYA_API const bool physics_world_is_paused();

/// Physics world functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Physics body functions

/// Create and allocate a physics body using the information provided in the given `desc`, 
/// returning back a valid `PhysicsBodyID` to be used later.
FREYA_API PhysicsBodyID physics_body_create(const PhysicsBodyDesc& desc);

/// Destroy the given `body`, deallocating any memory and disabling it in the world. 
///
/// @NOTE: Make sure to call `physics_world_remove_body` _before_ this function.
FREYA_API void physics_body_destroy(PhysicsBodyID& body);

/// Set the position of the given `body` to `position` and the roation (in radians) to `rotation`.
///
/// @NOTE: This works as a teleport and is generally more expensive. 
/// Use with caution.
FREYA_API void physics_body_set_transform(PhysicsBodyID& body, const Vec2 position, const f32 rotation);

/// Set the linear velocity of the given `body` to `velocity`.
FREYA_API void physics_body_set_linear_velocity(PhysicsBodyID& body, const Vec2 velocity);

/// Set the angular velocity of the given `body` to `velocity`.
FREYA_API void physics_body_set_angular_velocity(PhysicsBodyID& body, const f32 velocity);

/// Set the active state of the given `body` to `active`.
///
/// @NOTE: Activating a body will add the given `body` to the simulation. 
/// Disabling it, on the other hand, will _remove_ it from the simulation.
/// Meaning, both operation will be _very_ expensive. Use this sparingly.
FREYA_API void physics_body_set_active(PhysicsBodyID& body, const bool active);

/// Set the internal user data of the given `body` to `user_data`.
FREYA_API void physics_body_set_user_data(PhysicsBodyID& body, const void* user_data);

/// Set the gravity factor of the given `body` to `gravity_factor`.
FREYA_API void physics_body_set_gravity_factor(PhysicsBodyID& body, const f32 factor);

/// Set the body type of the given `body` to `type`.
FREYA_API void physics_body_set_type(PhysicsBodyID& body, const PhysicsBodyType type);

/// Apply force `force` to the given `body`. 
FREYA_API void physics_body_apply_force(PhysicsBodyID& body, const Vec2 force);

/// Apply force `force` at `point` to the given `body`. 
FREYA_API void physics_body_apply_force_at(PhysicsBodyID& body, const Vec2 force, const Vec2 point);

/// Apply torque force `torque` to the given `body`. 
FREYA_API void physics_body_apply_torque(PhysicsBodyID& body, const f32 torque);

/// Apply impulse force `impulse` to the given `body`. 
FREYA_API void physics_body_apply_impulse(PhysicsBodyID& body, const Vec2 impulse);

/// Apply impulse force `impulse` at `point` to the given `body`. 
FREYA_API void physics_body_apply_impulse_at(PhysicsBodyID& body, const Vec2 impulse, const Vec2 point);

/// Apply angluar impulse force `impulse` to the given `body`. 
FREYA_API void physics_body_apply_angular_impulse(PhysicsBodyID& body, const f32 impulse);

/// Retrieve the position of the given `body`.
FREYA_API const Vec2 physics_body_get_position(const PhysicsBodyID& body);

/// Retrieve the center of mass position of the given `body`.
FREYA_API const Vec2 physics_body_get_com_position(const PhysicsBodyID& body);

/// Retrieve the rotation (in radians) of the given `body`.
FREYA_API const f32 physics_body_get_rotation(const PhysicsBodyID& body);

/// Retrieve the linear velocity of the given `body`.
FREYA_API const Vec2 physics_body_get_linear_velocity(const PhysicsBodyID& body);

/// Retrieve the angular velocity of the given `body`.
FREYA_API const f32 physics_body_get_angular_velocity(const PhysicsBodyID& body);

/// Retrieve whether the given `body` is currently active or not.
FREYA_API const bool physics_body_is_active(const PhysicsBodyID& body);

/// Retrieve internal user data of the given `body`.
FREYA_API void* physics_body_get_user_data(const PhysicsBodyID& body);

/// Retrieve the gravity factor of the given `body`.
FREYA_API const f32 physics_body_get_gravity_factor(const PhysicsBodyID& body);

/// Retrieve the body type of the given `body`.
FREYA_API const PhysicsBodyType physics_body_get_type(const PhysicsBodyID& body);

/// Physics body functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Collider functions

/// Add a box collider to `body` using the information in `desc` with 
/// the `extents` being the size of the box collider in unit scale.
FREYA_API ColliderID collider_create(PhysicsBodyID& body, const ColliderDesc& desc, const Vec2& extents);

/// Set the density of the given `collider` to `density`.
FREYA_API void collider_set_density(ColliderID& collider, const f32 density);

/// Set the friction of the given `collider` to `friction`.
FREYA_API void collider_set_friction(ColliderID& collider, const f32 friction);

/// Set the restitution of the given `collider` to `restitution`.
FREYA_API void collider_set_restitution(ColliderID& collider, const f32 restitution);

/// Set both the layer and the mask layers of the given `collider` 
/// to `layer` and `mask_layers`.
FREYA_API void collider_set_layers(ColliderID& collider, const u64 layer, const u64 mask_layers);

/// Enable/disable contact events of the given `collider`.
FREYA_API void collider_enable_hit_events(ColliderID& collider, const bool enabled);

/// Get the information of `collider` in the form of a `ColliderDesc`
FREYA_API ColliderDesc collider_get_desc(ColliderID& collider);

/// Collider functions
///---------------------------------------------------------------------------------------------------------------------

} // End of freya

//////////////////////////////////////////////////////////////////////////

#include "freya_physics.h"
#include "freya_math.h"
#include "freya_logger.h"
#include "freya_event.h"
#include "freya_render.h"

#include <box2d/box2d.h>

//////////////////////////////////////////////////////////////////////////

namespace freya { // Start of freya

///---------------------------------------------------------------------------------------------------------------------
/// PhysicsWorld
struct PhysicsWorld {
  b2WorldId id; 
  b2DebugDraw draw_def;

  bool is_paused = false;
  bool is_debug  = false;
};

static PhysicsWorld s_world{};
/// PhysicsWorld
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Private functions

static Vec2 b2vec_to_vec(const b2Vec2& vec) {
  return Vec2(vec.x, vec.y) * PHYSICS_METERS_TO_PIXELS;
}

static b2Vec2 vec_to_b2vec(const Vec2& vec) {
  return b2Vec2(vec.x * PHYSICS_PIXELS_TO_METERS, vec.y * PHYSICS_PIXELS_TO_METERS);
}

static PhysicsBodyType b2_type_to_body_type(const b2BodyType& type) {
  switch(type) {
    case b2_staticBody:
      return PHYSICS_BODY_STATIC;
    case b2_dynamicBody:
      return PHYSICS_BODY_DYNAMIC;
    case b2_kinematicBody:
      return PHYSICS_BODY_KINEMATIC;
    default: 
      return PHYSICS_BODY_STATIC;
  }
}

static b2BodyType body_type_to_b2_type(const PhysicsBodyType& type) {
  switch(type) {
    case PHYSICS_BODY_STATIC:
      return b2_staticBody;
    case PHYSICS_BODY_DYNAMIC:
      return b2_dynamicBody;
    case PHYSICS_BODY_KINEMATIC:
      return b2_kinematicBody;
    default: 
      return b2_staticBody;
  }
}

static b2ShapeDef define_shape_def(const ColliderDesc& desc) {
  b2ShapeDef shape_def = b2DefaultShapeDef();

  shape_def.filter              = b2DefaultFilter();
  shape_def.filter.categoryBits = (u64)desc.layer;
  shape_def.filter.maskBits     = (u64)desc.mask_layers;

  shape_def.density             = desc.density;
  shape_def.material.friction   = desc.friction;
  shape_def.material.restitution = desc.restitution;

  shape_def.isSensor            = desc.is_sensor;
  shape_def.enableSensorEvents  = true;
  shape_def.enableContactEvents = true;
  shape_def.enableHitEvents     = false;

  return shape_def;
}

/// Private functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Callbacks

static f32 on_ray_cast_hit(b2ShapeId shape, b2Vec2 point, b2Vec2 normal, f32 fraction, void* context) {
  // Build the result

  RayCastResult result = {
    .body = b2Shape_GetBody(shape),

    .point    = b2vec_to_vec(point),
    .normal   = b2vec_to_vec(normal),
    .fraction = fraction,
  };

  // Dispatch en event

  Event event = {
    .type        = EVENT_PHYSICS_RAYCAST_HIT,
    .cast_result = result,
  };
  event_dispatch(event);

  // Done!
  return fraction;
}

static void b2draw_circle(b2Transform transform, f32 radius, b2HexColor b2color, void* context) {
  Color color   = Color(1.0f, 0.0f, 1.0f, 0.5f);
  Vec2 position = b2vec_to_vec(transform.p); 

  renderer_draw_debug_circle(position, radius * 100.0f, color);
}

static void b2draw_polygon(b2Transform transform, const b2Vec2* vertices, i32 vertex_count, f32 radius, b2HexColor b2color, void* context) {
  Color color = Color(1.0f, 0.0f, 1.0f, 0.5f);

  b2Vec2 size = vertices[0];
  for(i32 i = 1; i < vertex_count; i++) {
    size = b2Max(size, vertices[i]);
  }

  renderer_draw_debug_quad(b2vec_to_vec(transform.p), Vec2(size.x, size.y) * 100.0f, 0.0f, color);
}

static void b2draw_point(b2Vec2 p, float size, b2HexColor b2color, void* context) {
  Color color   = Color(1.0f, 0.0f, 1.0f, 0.5f);
  Vec2 position = b2vec_to_vec(p); 

  renderer_draw_debug_quad(position, Vec2(size) * 100.0f, 0.0f, color);
}

/// Callbacks
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Physics world functions

void physics_world_init(const Vec2& gravity) {
  // Def init

  b2WorldDef world_def = b2DefaultWorldDef();
  world_def.gravity    = vec_to_b2vec(gravity);

  // World init
  s_world.id = b2CreateWorld(&world_def);

  // Debug draw def init
  
  s_world.draw_def            = b2DefaultDebugDraw();
  s_world.draw_def.drawShapes = true;

  s_world.draw_def.DrawSolidCircleFcn  = b2draw_circle;
  s_world.draw_def.DrawSolidPolygonFcn = b2draw_polygon;
  s_world.draw_def.DrawPointFcn        = b2draw_point;

  // Done!
  FREYA_LOG_INFO("Successfully initialized the physics world");
}

void physics_world_shutdown() {
  b2DestroyWorld(s_world.id);

  FREYA_LOG_INFO("The physics world was successfully shutdown");
}

void physics_world_step(const f32 delta_time, const i32 sub_steps) {
  if(s_world.is_paused) {
    return;
  }

  // Step the physics world
  b2World_Step(s_world.id, delta_time, sub_steps);
  
  // Draw the debug mode (if enabled)

  if(s_world.is_debug) {
    b2World_Draw(s_world.id, &s_world.draw_def);
  }

  //
  // Handle contact events
  //

  b2ContactEvents events = b2World_GetContactEvents(s_world.id);

  // Begin contact events

  for(i32 i = 0; i < events.beginCount; i++) {
    b2ContactBeginTouchEvent* event = events.beginEvents + i;
    CollisionData coll_data         = {};

    // Get the bodies attached to the shapes 

    coll_data.body1  = b2Shape_GetBody(event->shapeIdA);
    coll_data.body2  = b2Shape_GetBody(event->shapeIdB);
    coll_data.normal = b2vec_to_vec(event->manifold.normal); 

    // Dispatch an event 

    Event coll_event = {
      .type           = EVENT_PHYSICS_CONTACT_ADDED, 
      .collision_data = coll_data,
    };
    event_dispatch(coll_event);
  }

  // End contact events

  for(i32 i = 0; i < events.endCount; i++) {
    b2ContactEndTouchEvent* event = events.endEvents + i;
    CollisionData coll_data       = {};

    // Get the bodies attached to the shapes (if they're available)
    
    if(b2Shape_IsValid(event->shapeIdA) && b2Shape_IsValid(event->shapeIdB)) {
      coll_data.body1 = b2Shape_GetBody(event->shapeIdA);
      coll_data.body2 = b2Shape_GetBody(event->shapeIdB);
    }

    // Dispatch an event 

    Event coll_event = {
      .type           = EVENT_PHYSICS_CONTACT_REMOVED, 
      .collision_data = coll_data,
    };
    event_dispatch(coll_event);
  }

  // 
  // Handle sensor contact events
  //

  b2SensorEvents sensor_events = b2World_GetSensorEvents(s_world.id);

  // Begin contact events

  for(i32 i = 0; i < sensor_events.beginCount; i++) {
    b2SensorBeginTouchEvent* event = sensor_events.beginEvents + i;
    SensorCollisionData coll_data  = {};

    // Get the bodies attached to the shapes 

    if(b2Shape_IsValid(event->sensorShapeId) && b2Shape_IsValid(event->visitorShapeId)) {
      coll_data.sensor_body  = b2Shape_GetBody(event->sensorShapeId);
      coll_data.visitor_body = b2Shape_GetBody(event->visitorShapeId);
    }

    // Dispatch an event 

    Event coll_event = {
      .type        = EVENT_PHYSICS_SENSOR_CONTACT_ADDED, 
      .sensor_data = coll_data,
    };
    event_dispatch(coll_event);
  }

  // End contact events

  for(i32 i = 0; i < sensor_events.endCount; i++) {
    b2SensorEndTouchEvent* event  = sensor_events.endEvents + i;
    SensorCollisionData coll_data = {};

    // Get the bodies attached to the shapes (if they're available)
    
    if(b2Shape_IsValid(event->sensorShapeId) && b2Shape_IsValid(event->visitorShapeId)) {
      coll_data.sensor_body  = b2Shape_GetBody(event->sensorShapeId);
      coll_data.visitor_body = b2Shape_GetBody(event->visitorShapeId);
    }

    // Dispatch an event 

    Event coll_event = {
      .type        = EVENT_PHYSICS_SENSOR_CONTACT_REMOVED, 
      .sensor_data = coll_data,
    };
    event_dispatch(coll_event);
  }
}

void physics_world_cast_ray(const RayCastDesc& cast_desc) {
  // Build the filter

  b2QueryFilter filter = b2DefaultQueryFilter();
  filter.categoryBits  = (u64)cast_desc.layer;
  filter.maskBits      = (u64)cast_desc.mask_layers;

  // Cast a ray into the world

  b2World_CastRay(s_world.id, 
                  vec_to_b2vec(cast_desc.origin), 
                  vec_to_b2vec(cast_desc.direction) * cast_desc.distance,
                  filter, 
                  on_ray_cast_hit, 
                  nullptr);
}

void physics_world_add_explosion(const ExplosionDesc& desc) {
  // Explosion def init
  
  b2ExplosionDef def   = b2DefaultExplosionDef();
  def.position         = vec_to_b2vec(desc.position);
  def.radius           = desc.radius;
  def.falloff          = desc.fall_off;
  def.impulsePerLength = desc.impulse_per_length;
  def.maskBits         = (u64)desc.mask_layers;

  // Explode!
  b2World_Explode(s_world.id, &def);
}

void physics_world_set_gravity(const Vec2& gravity) {
  b2World_SetGravity(s_world.id, vec_to_b2vec(gravity));
}

Vec2 physics_world_get_gravity() {
  return b2vec_to_vec(b2World_GetGravity(s_world.id));
}

void physics_world_toggle_paused() {
  s_world.is_paused = !s_world.is_paused;
}

void physics_world_toggle_debug() {
  s_world.is_debug = !s_world.is_debug;
}

const bool physics_world_is_paused() {
  return s_world.is_paused;
}

const bool physics_world_is_debug() {
  return s_world.is_debug;
}

/// Physics world functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Physics body functions

PhysicsBodyID physics_body_create(const PhysicsBodyDesc& desc) {
  // Body def init
  
  b2BodyDef body_def = b2DefaultBodyDef();
  body_def.position  = vec_to_b2vec(desc.position);
  body_def.type      = body_type_to_b2_type(desc.type);
  body_def.rotation  = b2MakeRot(desc.rotation); 

  body_def.linearVelocity  = vec_to_b2vec(desc.linear_velocity);
  body_def.angularVelocity = desc.angular_velocity;

  body_def.linearDamping  = desc.linear_damping;
  body_def.angularDamping = desc.angular_damping;
  body_def.gravityScale   = desc.gravity_factor;

  body_def.isAwake       = desc.is_active;
  body_def.fixedRotation = desc.rotation_fixed;
  body_def.userData      = (void*)desc.user_data;

  // Done!
  return b2CreateBody(s_world.id, &body_def);
}

void physics_body_destroy(PhysicsBodyID& body) {
  b2DestroyBody(body);
  body = b2_nullBodyId;
}

void physics_body_set_transform(PhysicsBodyID& body, const Vec2 position, const f32 rotation) {
  b2Body_SetTransform(body, vec_to_b2vec(position), b2MakeRot(rotation));
}

void physics_body_set_linear_velocity(PhysicsBodyID& body, const Vec2 velocity) {
  b2Body_SetLinearVelocity(body, vec_to_b2vec(velocity));
}

void physics_body_set_angular_velocity(PhysicsBodyID& body, const f32 velocity) {
  b2Body_SetAngularVelocity(body, velocity);
}

void physics_body_set_active(PhysicsBodyID& body, const bool active) {
  if(active) {
    b2Body_Enable(body);
  }
  else {
    b2Body_Disable(body);
  }
}

void physics_body_set_user_data(PhysicsBodyID& body, const uintptr user_data) {
  b2Body_SetUserData(body, (void*)user_data);
}

void physics_body_set_gravity_factor(PhysicsBodyID& body, const f32 factor) {
  b2Body_SetGravityScale(body, factor);
}

void physics_body_set_type(PhysicsBodyID& body, const PhysicsBodyType type) {
  b2Body_SetType(body, body_type_to_b2_type(type));
}

void physics_body_apply_force(PhysicsBodyID& body, const Vec2 force) {
  b2Body_ApplyForceToCenter(body, vec_to_b2vec(force), true);
}

void physics_body_apply_force_at(PhysicsBodyID& body, const Vec2 force, const Vec2 point) {
  b2Body_ApplyForce(body, vec_to_b2vec(force), vec_to_b2vec(point), true);
}

void physics_body_apply_torque(PhysicsBodyID& body, const f32 torque) {
  b2Body_ApplyTorque(body, torque, true);
}

void physics_body_apply_impulse(PhysicsBodyID& body, const Vec2 impulse) {
  b2Body_ApplyLinearImpulseToCenter(body, vec_to_b2vec(impulse), true);
}

void physics_body_apply_impulse_at(PhysicsBodyID& body, const Vec2 impulse, const Vec2 point) {
  b2Body_ApplyLinearImpulse(body, vec_to_b2vec(impulse), vec_to_b2vec(point), true);
}

void physics_body_apply_angular_impulse(PhysicsBodyID& body, const f32 impulse) {
  b2Body_ApplyAngularImpulse(body, impulse, true);
}

const Vec2 physics_body_get_position(const PhysicsBodyID& body) {
  return b2vec_to_vec(b2Body_GetPosition(body));
}

const Vec2 physics_body_get_com_position(const PhysicsBodyID& body) {
  return b2vec_to_vec(b2Body_GetWorldCenterOfMass(body));
}

const f32 physics_body_get_rotation(const PhysicsBodyID& body) {
  return b2Rot_GetAngle(b2Body_GetRotation(body));
}

const Vec2 physics_body_get_linear_velocity(const PhysicsBodyID& body) {
  return b2vec_to_vec(b2Body_GetLinearVelocity(body));
}

const f32 physics_body_get_angular_velocity(const PhysicsBodyID& body) {
  return b2Body_GetAngularVelocity(body);
}

const bool physics_body_is_active(const PhysicsBodyID& body) {
  return b2Body_IsEnabled(body);
}

const bool physics_body_is_valid(const PhysicsBodyID& body) {
  return b2Body_IsValid(body);
}

sizei physics_body_get_colliders_count(const PhysicsBodyID& body) {
  return b2Body_GetShapeCount(body);
}

void physics_body_get_colliders(const PhysicsBodyID& body, DynamicArray<ColliderID>& out_colliders) {
  // Inflate the array with the number of shapes first

  sizei shapes_count = physics_body_get_colliders_count(body);
  if(out_colliders.size() < shapes_count) {
    out_colliders.resize(shapes_count);
  }
 
  // Write the amount of shapes
  b2Body_GetShapes(body, out_colliders.data(), shapes_count);
}

uintptr physics_body_get_user_data(const PhysicsBodyID& body) {
  return (uintptr)b2Body_GetUserData(body);
}

const f32 physics_body_get_gravity_factor(const PhysicsBodyID& body) {
  return b2Body_GetGravityScale(body);
}

const PhysicsBodyType physics_body_get_type(const PhysicsBodyID& body) {
  return b2_type_to_body_type(b2Body_GetType(body));
}

/// Physics body functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Collider functions

ColliderID collider_create(PhysicsBodyID& body, const ColliderDesc& desc, const Vec2& extents) {
  // Shape def init
  b2ShapeDef shape_def = define_shape_def(desc);

  // Shape init
  
  b2Vec2 half_extents = vec_to_b2vec(extents / 2.0f); 
  b2Polygon shape     = b2MakeBox(half_extents.x, half_extents.y);

  // Done!
  return b2CreatePolygonShape(body, &shape_def, &shape);
}

ColliderID collider_create(PhysicsBodyID& body, const ColliderDesc& desc, const Vec2& center, const f32 radius) {
  // Shape def init
  b2ShapeDef shape_def = define_shape_def(desc);

  // Shape init
  
  b2Circle shape;
  shape.center = vec_to_b2vec(center);
  shape.radius = radius * PHYSICS_PIXELS_TO_METERS;

  // Done!
  return b2CreateCircleShape(body, &shape_def, &shape);
}

ColliderID collider_create(PhysicsBodyID& body, const ColliderDesc& desc, const Vec2& center1, const Vec2& center2, const f32 radius) {
  // Shape def init
  b2ShapeDef shape_def = define_shape_def(desc);

  // Shape init
  
  b2Capsule shape;
  shape.center1 = vec_to_b2vec(center1);
  shape.center2 = vec_to_b2vec(center2);
  shape.radius  = radius * PHYSICS_PIXELS_TO_METERS;

  // Done!
  return b2CreateCapsuleShape(body, &shape_def, &shape);
}

bool collider_test_point(ColliderID& collider, const Vec2& point) {
  return b2Shape_TestPoint(collider, vec_to_b2vec(point));
}

void collider_set_density(ColliderID& collider, const f32 density) {
  b2Shape_SetDensity(collider, density, true);
}

void collider_set_friction(ColliderID& collider, const f32 friction) {
  b2Shape_SetFriction(collider, friction);
}

void collider_set_restitution(ColliderID& collider, const f32 restitution) {
  b2Shape_SetRestitution(collider, restitution);
}

void collider_set_mask_layers(ColliderID& collider, const u64 layer, const u64 mask_layers) {
  b2Filter filter     = b2DefaultFilter();
  filter.categoryBits = layer; 
  filter.maskBits     = mask_layers; 

  b2Shape_SetFilter(collider, filter);
}

void collider_enable_hit_events(ColliderID& collider, const bool enabled) {
  b2Shape_EnableHitEvents(collider, enabled);
}

ColliderDesc collider_get_desc(ColliderID& collider) {
  return ColliderDesc {
    .density     = b2Shape_GetDensity(collider),
    .friction    = b2Shape_GetFriction(collider),
    .restitution = b2Shape_GetRestitution(collider),

    .layer       = (PhysicsObjectLayer)b2Shape_GetFilter(collider).categoryBits,
    .mask_layers = (PhysicsObjectLayer)b2Shape_GetFilter(collider).maskBits,

    .is_sensor = b2Shape_IsSensor(collider),
  };
}

PhysicsBodyID collider_get_body(ColliderID& collider) {
  return b2Shape_GetBody(collider);
}

/// Collider functions
///---------------------------------------------------------------------------------------------------------------------

} // End of freya

//////////////////////////////////////////////////////////////////////////

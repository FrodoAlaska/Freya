#include "freya_math.h"

//////////////////////////////////////////////////////////////////////////

namespace freya { // Start of freya

///---------------------------------------------------------------------------------------------------------------------
/// Basic collision functions

bool point_in_rect(const Vec2& point, const Vec2& position, const Vec2& size) {
  Vec2 min = position; 
  Vec2 max = position + size; 

  return (point.x >= min.x && point.x < max.x) && 
         (point.y >= min.y && point.y < max.y);
}

bool point_in_rect(const Vec2& point, const Rect2D& rect) {
  return point_in_rect(point, rect.position, rect.size);
}

bool point_in_circle(const Vec2& point, const Vec2& position, const f32 radius) {
  Vec2 diff = position - point;
  f32 len   = glm::length(diff); 

  return len < radius;
}

bool rect_in_rect(const Rect2D& r1, const Rect2D& r2) {
  Vec2 min1 = r1.position;
  Vec2 min2 = r2.position;
  
  Vec2 max1 = r1.position + r1.size;
  Vec2 max2 = r2.position + r2.size;

  return ((max1.x >= min2.x) && (min1.x <= max2.x)) &&
         ((max1.y >= min2.y) && (min1.y <= max2.y));
}

/// Basic collision functions
///---------------------------------------------------------------------------------------------------------------------

} // End of freya

//////////////////////////////////////////////////////////////////////////

#include "freya_math.h"

//////////////////////////////////////////////////////////////////////////

namespace freya { // Start of freya

/// ----------------------------------------------------------------------
/// Vertex functions

const sizei vertex_get_components_count(const i32 vertex_flags) {
  sizei count = 0; 

  // Vec2

  if(IS_BIT_SET(vertex_flags, VERTEX_COMPONENT_POSITION)) {
    count += 2;
  }

  if(IS_BIT_SET(vertex_flags, VERTEX_COMPONENT_NORMAL)) {
    count += 2;
  }

  if(IS_BIT_SET(vertex_flags, VERTEX_COMPONENT_TEXTURE_COORDS)) {
    count += 2;
  }

  // Vec4

  if(IS_BIT_SET(vertex_flags, VERTEX_COMPONENT_COLOR)) {
    count += 4;
  }

  return count;
}

const sizei vertex_get_stride(const i32 vertex_flags) {
  sizei stride = 0; 

  // Vec2

  if(IS_BIT_SET(vertex_flags, VERTEX_COMPONENT_POSITION)) {
    stride += sizeof(Vec2);
  }

  if(IS_BIT_SET(vertex_flags, VERTEX_COMPONENT_NORMAL)) {
    stride += sizeof(Vec2);
  }

  if(IS_BIT_SET(vertex_flags, VERTEX_COMPONENT_TEXTURE_COORDS)) {
    stride += sizeof(Vec2);
  }

  // Vec4

  if(IS_BIT_SET(vertex_flags, VERTEX_COMPONENT_COLOR)) {
    stride += sizeof(Vec4);
  }

  return stride;
}

/// Vertex functions
/// ----------------------------------------------------------------------

} // End of freya

//////////////////////////////////////////////////////////////////////////

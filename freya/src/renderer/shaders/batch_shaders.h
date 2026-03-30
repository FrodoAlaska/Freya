#pragma once

#include "freya_gfx.h"

inline freya::String generate_default_vertex_shader() {
  return 
    freya::String(FREYA_VERTEX_SHADER_HEADER) + // @TEMP (Shaders): Ew. Find a better way
    R"(
    // Layouts
    
    layout (location = 0) in vec2 aPos;
    layout (location = 1) in vec2 aNormal;
    layout (location = 2) in vec2 aTextureCoords;
    layout (location = 3) in vec4 aColor;
     
    // Outputs
    
    out VS_OUT {
      vec2 pixel_pos;
      vec2 normal;
      vec2 tex_coords;
      vec4 out_color;
    } vs_out;

    // Buffers
    
    layout(std140, binding = 0) uniform MatrixBuffer {
      mat4 u_ortho;
    };

    void main() {
      vs_out.pixel_pos  = aPos;
      vs_out.normal     = aNormal;
      vs_out.tex_coords = aTextureCoords;
      vs_out.out_color  = aColor;

      gl_Position = u_ortho * vec4(aPos, 0.0f, 1.0f);
    }
  )";
}

inline freya::GfxShaderDesc generate_batch_quad_shader() {
  return freya::GfxShaderDesc {
    .vertex_source = generate_default_vertex_shader(),
    .pixel_source  = 
      freya::String(FREYA_PIXEL_SHADER_HEADER) +
      R"(
      // Outputs
      layout (location = 0) out vec4 frag_color;
       
      // Inputs
      
      in VS_OUT {
        vec2 pixel_pos;
        vec2 normal;
        vec2 tex_coords;
        vec4 out_color;
      } fs_in;

      // Uniforms
      uniform sampler2D u_texture;

      void main() {
        frag_color = texture(u_texture, fs_in.tex_coords) * fs_in.out_color;
      }
    )"
  };
}

inline freya::String generate_debug_vertex_shader() {
  return 
    freya::String(FREYA_VERTEX_SHADER_HEADER) + // @TEMP (Shaders): Ew. Find a better way
    R"(
    // Layouts
    
    layout (location = 0) in vec2 aPos;
    layout (location = 1) in vec4 aColor;

    // Outputs
    
    out VS_OUT {
      vec4 color;
    } vs_out;

    // Buffers
    
    layout(std140, binding = 0) uniform MatrixBuffer {
      mat4 u_ortho;
    };

    void main() {
      vs_out.color = aColor;
      gl_Position  = u_ortho * vec4(aPos, 0.0f, 1.0f);
    }
  )";
}

inline freya::GfxShaderDesc generate_debug_shader() {
  return freya::GfxShaderDesc {
    .vertex_source = generate_debug_vertex_shader(),
    .pixel_source  = 
      freya::String(FREYA_PIXEL_SHADER_HEADER) +
      R"(
      // Outputs
      layout (location = 0) out vec4 frag_color;
      
      // Inputs
      
      in VS_OUT {
        vec4 color;
      } fs_in;

      void main() {
        frag_color = fs_in.color;
      }
    )"
  };
}

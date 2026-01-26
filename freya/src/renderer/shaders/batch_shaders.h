#pragma once

#include "freya_gfx.h"

inline const char* generate_default_vertex_shader() {
  return R"(
    #version 460 core
   
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
      vs_out.pixel_pos     = aPos;
      vs_out.normal        = aNormal;
      vs_out.tex_coords    = aTextureCoords;
      vs_out.out_color     = aColor;

      gl_Position = u_ortho * vec4(aPos, 0.0f, 1.0f);
    }
  )";
}

inline freya::GfxShaderDesc generate_batch_quad_shader() {
  return freya::GfxShaderDesc {
    .vertex_source = generate_default_vertex_shader(),
 
    .pixel_source = R"(
      #version 460 core
     
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

inline const char* generate_debug_vertex_shader() {
  return R"(
    #version 460 core
   
    // Layouts
    
    layout (location = 0) in vec2 aPos;
    layout (location = 1) in vec2 aTextureCoords;

    // Outputs
    
    out VS_OUT {
      vec2 tex_coords;
    } vs_out;

    // Buffers
    
    layout(std140, binding = 0) uniform MatrixBuffer {
      mat4 u_ortho;
    };
    
    // Uniforms
    uniform mat4 u_model;

    void main() {
      vs_out.tex_coords = aTextureCoords;

      gl_Position = u_ortho * u_model * vec4(aPos, 0.0f, 1.0f);
    }
  )";
}

inline freya::GfxShaderDesc generate_quad_shader() {
  return freya::GfxShaderDesc {
    .vertex_source = generate_debug_vertex_shader(),
 
    .pixel_source = R"(
      #version 460 core
     
      // Outputs
      layout (location = 0) out vec4 frag_color;
      
      // Inputs
      
      in VS_OUT {
        vec2 tex_coords;
      } fs_in;

      // Uniforms
      uniform vec4 u_color;

      void main() {
        frag_color = u_color;
      }
    )"
  };
}

inline freya::GfxShaderDesc generate_circle_shader() {
  return freya::GfxShaderDesc {
    .vertex_source = generate_debug_vertex_shader(),
 
    .pixel_source = R"(
      #version 460 core
     
      // Outputs
      layout (location = 0) out vec4 frag_color;
      
      // Inputs
      
      in VS_OUT {
        vec2 tex_coords;
      } fs_in;

      // Uniforms
      uniform vec4 u_color;

      void main() {
        vec2 uv    = fs_in.tex_coords.xy * 2.0 - 1.0;
        float dist = 1.0 - length(uv);

        if(dist < 0.0) {
          discard;
        }

        frag_color = u_color;
      }
    )"
  };
}

inline freya::GfxShaderDesc generate_polygon_shader() {
  return freya::GfxShaderDesc {
    .vertex_source = generate_debug_vertex_shader(),
 
    .pixel_source = R"(
      #version 460 core
      
      // Defines
      
      #define PI     3.14159265359
      #define TWO_PI 6.28318530718
     
      // Outputs
      layout (location = 0) out vec4 frag_color;
      
      // Inputs
      
      in VS_OUT {
        vec2 tex_coords;
      } fs_in;

      // Uniforms
      
      uniform vec4 u_color;
      uniform float u_radius;
      uniform int u_sides;

      void main() {
        vec2 uv = fs_in.tex_coords.xy * 2.0 - 1.0;

        float angle  = atan(uv.x, uv.y);
        float radius = TWO_PI / u_sides;

        float d   = cos(floor(0.5 + angle / u_radius) * u_radius - angle) * length(uv);
        float val = 1.0 - smoothstep(0.4, 0.41, d);

        if(val <= 0.0) {
          discard;
        }
        
        frag_color = u_color;
      }
    )"
  };
}

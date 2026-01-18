#pragma once

#include "freya_gfx.h"

inline freya::GfxShaderDesc generate_batch_quad_shader() {
  return freya::GfxShaderDesc {
    .vertex_source = R"(
      #version 460 core
     
      // Layouts
      
      layout (location = 0) in vec2 aPos;
      layout (location = 1) in vec2 aNormal;
      layout (location = 2) in vec2 aTextureCoords;
      layout (location = 3) in vec4 aColor;
      layout (location = 4) in float aTextureIndex;
    
      // Outputs
      
      out VS_OUT {
        vec2 pixel_pos;
        vec2 normal;
        vec2 tex_coords;
        vec4 out_color;
   
        flat int texture_index;
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
        vs_out.texture_index = int(aTextureIndex);

        gl_Position = u_ortho * vec4(aPos, 0.0f, 1.0f);
      }
    )",
 
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
   
        flat int texture_index;
      } fs_in;

      // Uniforms
      uniform sampler2D u_textures[32];

      void main() {
        frag_color = texture(u_textures[fs_in.texture_index], fs_in.tex_coords) * fs_in.out_color;
      }
    )"
  };
}

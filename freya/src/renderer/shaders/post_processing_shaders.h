#pragma once

#include "freya_gfx.h"

inline freya::GfxShaderDesc generate_default_screen_space_shader() {
  return freya::GfxShaderDesc {
    .vertex_source = 
      freya::String(FREYA_VERTEX_SHADER_HEADER) + // @TEMP (Shaders): Ew. Find a better way
      R"(
      // Layouts
      
      layout (location = 0) in vec2 aPos;
      layout (location = 1) in vec2 aTextureCoords;
       
      // Outputs
      
      out VS_OUT {
        vec2 tex_coords;
      } vs_out;

      void main() {
        vs_out.tex_coords = aTextureCoords;
        gl_Position       = vec4(aPos, 0.0f, 1.0f);
      }
    )",
    .pixel_source = 
      freya::String(FREYA_PIXEL_SHADER_HEADER) +
      R"(
      // Outputs
      layout (location = 0) out vec4 frag_color;
       
      // Inputs
      
      in VS_OUT {
        vec2 tex_coords;
      } fs_in;

      // Uniforms
      uniform sampler2D u_texture;

      void main() {
        frag_color = texture(u_texture, fs_in.tex_coords);
      }
    )"
  };
}

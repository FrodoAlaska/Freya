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

inline freya::GfxShaderDesc generate_hdr_shader() {
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

      const float GAMMA = 2.0;

      // Uniforms

      layout (binding = 0) uniform sampler2D u_input;
      uniform float u_exposure;
      
      void main() {
        vec3 hdr_color = texture(u_input, fs_in.tex_coords).rgb;
        vec3 mapped    = vec3(1.0) - exp(-hdr_color * u_exposure);
     
        mapped     = pow(mapped, vec3(1.0 / GAMMA));
        frag_color = vec4(mapped, 1.0);
      }
    )"
  };
}

inline freya::GfxShaderDesc generate_blur_shader() {
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
      layout (binding = 0) uniform sampler2D u_input;
     
      vec3 kernel(float[9] kernel_values) {
        // Define the offsets

        const float offset = 1 / 300.0f;
        vec2 offsets[9]    = vec2[](
          vec2(-offset, offset), // Top-left
          vec2(0.0f, offset),    // Top-center
          vec2(offset, offset),  // Top-right

          vec2(-offset, 0.0f),   // Center-left
          vec2(0.0f, 0.0f),      // Center
          vec2(offset, 0.0f),    // Center-right

          vec2(-offset, -offset), // Bottom-left
          vec2(0.0f, -offset),    // Bottom-center
          vec2(offset, -offset)   // Bottom-right
         );

        // Get the texture values in the whole 3x3

        vec3 sample_tex[9];
        for(int i = 0; i < 9; i++) {
          sample_tex[i] = vec3(texture(u_input, fs_in.tex_coords.st + offsets[i]));
        }

        // Apply the kernal values 

        vec3 color = vec3(0.0f);
        for(int i = 0; i < 9; i++) {
          color += sample_tex[i] * kernel_values[i];
        }
        
        // Done!
        return color;
      }

      void main() {
        const float mul = 16.0f;
        float k[9] = float[](
          1.0f / mul, 2.0f / mul, 1.0f / mul,
          2.0f / mul, 4.0f / mul, 2.0f / mul,
          1.0f / mul, 2.0f / mul, 1.0f / mul 
        );

        frag_color = vec4(kernel(k), 1.0);
      }
    )"
  };
}

inline freya::GfxShaderDesc generate_greyscale_shader() {
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
      layout (binding = 0) uniform sampler2D u_input;
      
      void main() {
        vec4 tex_color = texture(u_input, fs_in.tex_coords);
        float average  = (0.2126 * tex_color.r) + (0.7152 * tex_color.g) + (0.0722 * tex_color.b);

        frag_color = vec4(average, 1.0);
      }
    )"
  };
}

inline freya::GfxShaderDesc generate_vignette_shader() {
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
      
      layout (binding = 0) uniform sampler2D u_input;
      uniform float u_intensity;
      
      void main() {
        /// @NOTE: 
        ///
        /// Taken from: https://www.shadertoy.com/view/lsKSWR
        ///

        vec2 uv = fs_in.tex_coords;
        uv     *= 1.0 - uv.xy;

        float vig = uv.x * uv.y * u_intensity;
        vig       = pow(vig, 0.25);

        frag_color = texture(u_input, fs_in.tex_coords) * vec4(vec3(vig), 1.0);
      }
    )"
  };
}

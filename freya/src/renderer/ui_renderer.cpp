#include "freya_render.h"
#include "freya_ui.h"
#include "freya_assets.h"
#include "freya_event.h"
#include "freya_logger.h"

#include "shaders/ui_shader.h"

#include <RmlUi/Core.h>
#include <RmlUi/Core/SystemInterface.h>
#include <RmlUi/Core/RenderInterface.h>
#include <RmlUi/Core/Types.h>
#include <RmlUi/Core/Log.h>

//////////////////////////////////////////////////////////////////////////

namespace freya { // Start of freya

///---------------------------------------------------------------------------------------------------------------------
/// Consts

const sizei VERTEX_BUFFER_SIZE = KiB(256);
const sizei INDEX_BUFFER_SIZE  = KiB(256);

/// Consts
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// ShaderID
enum ShaderID {
  SHADER_TEXTURE = 0, 
  SHADER_COLOR,

  SHADERS_MAX,
};
/// ShaderID
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Forward declarations

class FRSystemInterface;
class FRRenderInterface;

/// Forward declarations
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// MatricesUniformInterface
struct MatricesUniformInterface {
  Mat4 projection = Mat4(1.0f); 
  Mat4 transform  = Mat4(1.0f); 

  Vec2 translation = Vec2(0.0f);
  u8 __padding[8];
};
/// MatricesUniformInterface
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// UIBatch
struct UIBatch {
  sg_buffer vertex_buffer = {};
  sg_buffer index_buffer  = {};

  u32 count = 0;
};
/// UIBatch
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// UIRenderer
struct UIRenderer {
  FRSystemInterface* system_interface; 
  FRRenderInterface* render_interface;

  Window* window;

  sg_pass_action pass_action = {};
  sg_pass pass               = {};

  sg_sampler sampler = {};

  sg_pipeline pipeline  = {}; 
  Texture white_texture = {};

  DynamicArray<Texture> textures;
  DynamicArray<UIBatch> batches;

  MatricesUniformInterface uniform;

  AssetGroupID group_id;
};

static UIRenderer s_renderer;
/// UIRenderer
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// FRSystemInterface
class FRSystemInterface : public Rml::SystemInterface {
public:
  FRSystemInterface() = default;

public:
  double GetElapsedTime() override {
    return clock_get_time();
  }

  bool LogMessage(Rml::Log::Type type, const Rml::String& message) override {
    switch(type) {
      case Rml::Log::Type::LT_ERROR:
        FREYA_LOG_ERROR("RML-ERROR: %s", message.c_str());
        return false;
      case Rml::Log::Type::LT_ASSERT:
        FREYA_DEBUG_ASSERT(false, message.c_str());
        return false;
      case Rml::Log::Type::LT_WARNING:
        FREYA_LOG_WARN("RML-WARNING: %s", message.c_str());
        break;
      case Rml::Log::Type::LT_INFO:
        FREYA_LOG_INFO("RML-INFO:  %s", message.c_str());
        break;
    }

    return true;
  }
};
/// FRSystemInterface
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// FRRenderInterface
class FRRenderInterface : public Rml::RenderInterface {
public:
  FRRenderInterface(UIRenderer& ui_renderer)
    :renderer(ui_renderer)
  {}

public:
  Rml::CompiledGeometryHandle CompileGeometry(Rml::Span<const Rml::Vertex> vertices, Rml::Span<const int> indices) override {
    UIBatch batch = {}; 
     
    // Vertex buffer init

    sg_buffer_desc vertex_desc = {};

    vertex_desc.usage.vertex_buffer = true;
    vertex_desc.data                = {vertices.data(), vertices.size() * sizeof(Rml::Vertex)};

    batch.vertex_buffer = sg_make_buffer(vertex_desc); 

    // Index buffer init

    sg_buffer_desc index_desc = {};

    index_desc.usage.index_buffer = true;
    index_desc.data               = {indices.data(), indices.size() * sizeof(i32)};

    batch.index_buffer = sg_make_buffer(index_desc); 

    // Done!
    
    batch.count = (u32)indices.size();
    renderer.batches.push_back(batch);

    return (Rml::CompiledGeometryHandle)(renderer.batches.size());
  }
  
  void RenderGeometry(Rml::CompiledGeometryHandle geometry, Rml::Vector2f translation, Rml::TextureHandle tex_handle) override {
    // Set up the bindings of the pass 
    sg_bindings bindings = {};

    // Use the resources of the draw call

    UIBatch& batch = renderer.batches[(sizei)(geometry - 1)];

    bindings.vertex_buffers[0] = batch.vertex_buffer;
    bindings.index_buffer      = batch.index_buffer;

    bindings.samplers[0] = renderer.white_texture.sampler;

    Texture* texture = &renderer.textures[0]; // Assume it's the white texture
    if(tex_handle > 0) {
      texture = &renderer.textures[(sizei)(tex_handle - 1)];
    }

    bindings.views[0] = texture->view;

    // Set the translation 
    renderer.uniform.translation = Vec2(translation.x, translation.y);

    // Apply the bindings and the pipeline

    sg_apply_pipeline(renderer.pipeline);
    sg_apply_uniforms(UB_Matrices, SG_RANGE(renderer.uniform));
    sg_apply_bindings(bindings);

    // Render the screen-space post-process effect
    sg_draw(0, batch.count, 1);
  }
  
  void ReleaseGeometry(Rml::CompiledGeometryHandle geometry) override {
    UIBatch& batch = renderer.batches[(sizei)(geometry - 1)];

    sg_uninit_buffer(batch.vertex_buffer);
    sg_uninit_buffer(batch.index_buffer);
  }

  Rml::TextureHandle LoadTexture(Rml::Vector2i& texture_dimensions, const Rml::String& source) override {
    // Use the preset asset group to retrieve the texture by name
   
    String name        = filepath_stem(source);
    AssetID texture_id = asset_group_get_id(renderer.group_id, name);
    Texture& texture   = asset_group_get_texture(texture_id);

    // Write back the dimensions of the texture to Rml

    texture_dimensions.x = texture.size.x;
    texture_dimensions.y = texture.size.y;

    // Done!
    
    renderer.textures.push_back(texture); 
    return (Rml::TextureHandle)(renderer.textures.size());
  }

  Rml::TextureHandle GenerateTexture(Rml::Span<const Rml::byte> source, Rml::Vector2i source_dimensions) override {
    // Convert the given data to our format

    sg_image_desc image_desc = {}; 
    
    image_desc.width  = source_dimensions.x;
    image_desc.height = source_dimensions.y;

    image_desc.pixel_format = SG_PIXELFORMAT_RGBA8;

    image_desc.data.mip_levels[0].ptr  = source.data();
    image_desc.data.mip_levels[0].size = source.size();

    sg_sampler_desc sampler_desc = {};

    sampler_desc.min_filter = SG_FILTER_NEAREST;
    sampler_desc.mag_filter = SG_FILTER_NEAREST;

    // Add it to the asset group
    AssetID texture_id = asset_group_push_texture(renderer.group_id, image_desc, sampler_desc); 

    // Done!
    
    renderer.textures.push_back(asset_group_get_texture(texture_id)); 
    return (Rml::TextureHandle)(renderer.textures.size());
  }

  void ReleaseTexture(Rml::TextureHandle texture) override {
    renderer.textures[(sizei)(texture - 1)] = Texture{};
  }

  void EnableScissorRegion(bool enable) override {
    // @TODO (UI)
  }

  void SetScissorRegion(Rml::Rectanglei region) override {
    Rml::Vector2i position = region.Position();
    Rml::Vector2i size     = region.Size();

    sg_apply_scissor_rect(position.x, position.y, size.x, size.y, true);
  }

  void SetTransform(const Rml::Matrix4f* transform) override {
    if(!transform) {
      renderer.uniform.transform = Mat4(1.0f);
      return;
    }

    Rml::Vector4f colomn1 = (*transform)[0];
    Rml::Vector4f colomn2 = (*transform)[1];
    Rml::Vector4f colomn3 = (*transform)[2];
    Rml::Vector4f colomn4 = (*transform)[3];

    renderer.uniform.transform[0] = Vec4(colomn1.x, colomn1.y, colomn1.z, colomn1.w);
    renderer.uniform.transform[1] = Vec4(colomn2.x, colomn2.y, colomn2.z, colomn2.w);
    renderer.uniform.transform[2] = Vec4(colomn3.x, colomn3.y, colomn3.z, colomn3.w);
    renderer.uniform.transform[3] = Vec4(colomn4.x, colomn4.y, colomn4.z, colomn4.w);
  }

  Rml::CompiledShaderHandle CompileShader(const Rml::String& name, const Rml::Dictionary& parameters) override {
    // @TODO (UI)
    return 0;
  }
  
  void RenderShader(Rml::CompiledShaderHandle shader,
                    Rml::CompiledGeometryHandle geometry,
                    Rml::Vector2f translation,
                    Rml::TextureHandle texture) override {
    // @TODO (UI)
  }

  void ReleaseShader(Rml::CompiledShaderHandle shader) override {
    // @TODO (UI)
  }

public:
  UIRenderer& renderer;
};
/// FRRenderInterface
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// UI renderer functions

bool ui_renderer_init(Window* window) {
  FREYA_PROFILE_FUNCTION();

  // Window init
  s_renderer.window = window;

  //
  // Pipeline init
  //
  
  sg_pipeline_desc pipe_desc = {};

  pipe_desc.depth.compare       = SG_COMPAREFUNC_LESS_EQUAL;
  pipe_desc.depth.write_enabled = true;
  pipe_desc.index_type          = SG_INDEXTYPE_UINT32;

  AssetID shader_id = asset_group_push_shader(ASSET_CACHE_ID, *ui_shader_desc(sg_query_backend()));
  pipe_desc.shader  = asset_group_get_shader(shader_id); 

  pipe_desc.layout.attrs[0].format = SG_VERTEXFORMAT_FLOAT2;  // Position
  pipe_desc.layout.attrs[1].format = SG_VERTEXFORMAT_UBYTE4N; // Color
  pipe_desc.layout.attrs[2].format = SG_VERTEXFORMAT_FLOAT2;  // Texture coords

  pipe_desc.alpha_to_coverage_enabled = true;

  s_renderer.pipeline = sg_make_pipeline(pipe_desc);

  // 
  // Textures init 
  //

  sg_image_desc image_desc = {};

  image_desc.width        = 1;
  image_desc.height       = 1;
  image_desc.pixel_format = SG_PIXELFORMAT_RGBA8;

  u8 white_pixels[4]            = {255, 255, 255, 255};
  image_desc.data.mip_levels[0] = SG_RANGE(white_pixels);

  sg_sampler_desc sampler_desc = {};

  AssetID texture_id       = asset_group_push_texture(ASSET_CACHE_ID, image_desc, sampler_desc);
  s_renderer.white_texture = asset_group_get_texture(texture_id); 

  s_renderer.textures.push_back(s_renderer.white_texture);

  // Pre-allocating some memory for better performance
  s_renderer.batches.reserve(128);

  //
  // Interfaces init
  //

  s_renderer.system_interface = new FRSystemInterface();
  s_renderer.render_interface = new FRRenderInterface(s_renderer);

  Rml::SetSystemInterface(s_renderer.system_interface);
  Rml::SetRenderInterface(s_renderer.render_interface);
  
  Rml::Initialise();

  // Done!
  
  FREYA_LOG_INFO("Successfully initialized the ui renderer");
  return true;
}

void ui_renderer_shutdown() {
  FREYA_PROFILE_FUNCTION();

  // Interfaces deinit
  
  Rml::Shutdown();

  delete s_renderer.render_interface;
  delete s_renderer.system_interface;

  // Clearing all the resources

  s_renderer.textures.clear();
  s_renderer.batches.clear();

  // Done!
  FREYA_LOG_INFO("Successfully shutdown the ui renderer");
}

void ui_renderer_begin() {
  FREYA_PROFILE_FUNCTION();
  // @TODO
}

void ui_renderer_end() {
  FREYA_PROFILE_FUNCTION();
  // @TODO
}

void ui_renderer_apply_context(UIContext* ui_ctx) {
  FREYA_PROFILE_FUNCTION();

  // Calculating the orthographic matrix

  IVec2 size                    = window_get_size(s_renderer.window);
  s_renderer.uniform.projection = mat4_ortho(0.0f, (f32)size.x, (f32)size.y, 0.0f);
  
  // Set up the pass 

  const Color& col = renderer_get_clear_color();

  s_renderer.pass_action.colors[0].load_action = SG_LOADACTION_CLEAR;
  s_renderer.pass_action.colors[0].clear_value = {
    .r = col.r,
    .g = col.g,
    .b = col.b,
    .a = col.a,
  }; 

  s_renderer.pass.action    = s_renderer.pass_action;
  s_renderer.pass.swapchain = renderer_get_default_swapchain();

  // Begin the pass
  sg_begin_pass(&s_renderer.pass);

  // Render the context
  ui_context_render(ui_ctx);
   
  // End the pass and commit

  sg_end_pass();
  sg_commit();
}

void ui_renderer_set_asset_group(const AssetGroupID& group_id) {
  s_renderer.group_id = group_id;
}

bool ui_renderer_set_font(const String& font_name) {
  const AssetID& font_id = asset_group_get_id(s_renderer.group_id, font_name);
  Font* font             = asset_group_get_font(font_id);

  return Rml::LoadFontFace(font->font_data, font->name, Rml::Style::FontStyle::Normal);
}

/// UI renderer functions
///---------------------------------------------------------------------------------------------------------------------

} // End of freya

//////////////////////////////////////////////////////////////////////////

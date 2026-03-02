#include "app.h"

#include <freya.h>
#include <imgui/imgui.h>

/// ----------------------------------------------------------------------
/// Consts

const freya::u32 TEXTURE_WIDTH  = 256;
const freya::u32 TEXTURE_HEIGHT = 256;

/// Consts
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// App
struct freya::App {
  freya::Window* window;
  freya::Camera camera;
  freya::AssetGroupID group_id;

  freya::NoiseGenerator* generator;
  freya::DynamicArray<freya::Color> color_values;
  freya::GfxTexture* noise_texture;
};
/// App
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Private functions

static void generate_texture(freya::App* app) {
  // Setting the color values

  for(freya::i32 y = 0; y < TEXTURE_HEIGHT; y++) {
    for(freya::i32 x = 0; x < TEXTURE_WIDTH; x++) {
      freya::Vec2 coords = freya::Vec2((freya::f32)x, (freya::f32)y);
      freya::noise_generator_domain_warp(app->generator, coords);

      freya::f32 noise    = (freya::noise_generator_get(app->generator, coords) + 1) * 0.5f; // Changing the range from [-1 1] to [0 1]
      freya::Color& color = app->color_values[y * TEXTURE_WIDTH + x];

      color = freya::Color(noise);
    }
  } 
  
  // Update the pixels for the color texture

  freya::GfxTextureDesc& tex_desc = freya::gfx_texture_get_desc(app->noise_texture);

  tex_desc.width  = TEXTURE_WIDTH;
  tex_desc.height = TEXTURE_HEIGHT;
  tex_desc.data   = (void*)app->color_values.data();

  freya::gfx_texture_reload(app->noise_texture, tex_desc);
}

/// Private functions
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// App functions 

freya::App* app_init(const freya::Args& args, freya::Window* window) {
  // App init
  
  freya::App* app = new freya::App{};
  freya::renderer_set_clear_color(freya::Vec4(0.1f, 0.1f, 0.1f, 1.0f));

  // Window init
  app->window = window;

  // Editor init
  freya::gui_init(window);

  // Camera init
  
  freya::CameraDesc cam_desc = {
    .position = freya::Vec2(-800.0f, -450.0f),
    .zoom     = 0.01f,
  };
  freya::camera_create(app->camera, cam_desc);

  // Assets init

  app->group_id = freya::asset_group_create("app_assets");

  freya::GfxTextureDesc tex_desc = {
    .width  = TEXTURE_WIDTH, 
    .height = TEXTURE_HEIGHT,

    .type      = freya::GFX_TEXTURE_2D,
    .format    = freya::GFX_TEXTURE_FORMAT_RGBA32F,  
    .filter    = freya::GFX_TEXTURE_FILTER_MIN_MAG_NEAREST,  
    .wrap_mode = freya::GFX_TEXTURE_WRAP_CLAMP,
  };
  app->noise_texture = freya::asset_group_get_texture(freya::asset_group_push_texture(app->group_id, tex_desc));

  // Noise generator init
  
  app->generator = freya::noise_generator_create(freya::NoiseGeneratorDesc{});
  app->color_values.resize(TEXTURE_WIDTH * TEXTURE_HEIGHT);

  generate_texture(app);

  // Done!
  return app;
}

void app_shutdown(freya::App* app) {
  freya::noise_generator_destroy(app->generator);
  freya::asset_group_destroy(app->group_id);
  freya::gui_shutdown();

  delete app;
}

void app_update(freya::App* app, const freya::f32 delta_time) {
  // Quit the application when the specified exit key is pressed
  
  if(freya::input_key_pressed(freya::KEY_ESCAPE)) {
    freya::event_dispatch(freya::Event{.type = freya::EVENT_APP_QUIT});
    return;
  }

  // Re-generate the noise texture 

  if(freya::input_key_pressed(freya::KEY_SPACE)) {
    generate_texture(app);
  }
}

void app_render(freya::App* app) {
  // 2D render

  freya::renderer_begin(app->camera);

  freya::Transform transform = {
    .position = freya::Vec2(0.0f),
    .scale    = freya::Vec2(TEXTURE_WIDTH, TEXTURE_HEIGHT),
  };
  freya::renderer_queue_texture(app->noise_texture, transform);

  freya::renderer_end();

  // UI render
  
  freya::ui_renderer_begin();
  freya::ui_renderer_end();
}

void app_render_gui(freya::App* app) {
  freya::gui_begin(); 
 
  // Debug
  freya::gui_debug_info();

  // Editor

  freya::gui_begin_panel("Editor");

  freya::gui_edit_camera("Camera", &app->camera);
  freya::gui_edit_noise_generator("Nosie generator", app->generator);
  
  freya::gui_end_panel();

  freya::gui_end();
}

/// App functions 
/// ----------------------------------------------------------------------

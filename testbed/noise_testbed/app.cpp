#include "app.h"

#include <freya.h>
#include <imgui.h>

/// ----------------------------------------------------------------------
/// Consts

const freya::u32 TEXTURE_WIDTH  = 512;
const freya::u32 TEXTURE_HEIGHT = 512;

/// Consts
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// App
struct App {
  freya::Window* window;
  freya::AssetGroupID group_id;

  freya::EntityWorld world; 

  freya::EntityID noise_entt;
  freya::NoiseGenerator* gen; 
  freya::NoiseGeneratorDesc noise_desc;

  freya::DynamicArray<freya::Color> colors;
};

static App s_app;
/// App
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Private functions

static void generate_map() {
  // Set the noise data
 
  s_app.noise_desc = freya::noise_generator_get_desc(s_app.gen);
  freya::noise_generator_set_desc(s_app.gen, s_app.noise_desc);

  // Make sense of the noise

  for(freya::i32 i = 0; i < TEXTURE_HEIGHT; i++) {
    for(freya::i32 j = 0; j < TEXTURE_WIDTH; j++) {
      // Determining the terrain type by the noise value given

      freya::Vec2 coords = freya::Vec2(j, i);

      freya::f32 noise    = (freya::noise_generator_get(s_app.gen, coords) + 1) * 0.5f; // Going from [-1, 1] to [0, 1]
      freya::Color& color = s_app.colors[i * TEXTURE_WIDTH + j];

      color.r = noise;
      color.g = noise; 
      color.b = noise;
      color.a = 1.0f;
    }
  }

  // Update the pixels for the color texture

  freya::Texture& texture = freya::entity_get_component<freya::SpriteComponent>(s_app.world, s_app.noise_entt).texture;
  texture.size            = freya::IVec2(TEXTURE_WIDTH, TEXTURE_HEIGHT);

  sg_image_data data      = {}; 
  data.mip_levels[0].ptr  = (const void*)s_app.colors.data();
  data.mip_levels[0].size = s_app.colors.size() * sizeof(freya::Color);

  sg_update_image(texture.image, data);
}

/// Private functions
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// App functions 

bool app_init(const freya::Args& args, freya::Window* window) {
  // Renderer init
  
  freya::renderer_set_clear_color(freya::Vec4(0.1f, 0.1f, 0.1f, 1.0f));
  freya::renderer_sumbit_world(&s_app.world);

  freya::renderer_set_sort(true);

  // Window init
  s_app.window = window;

  // Editor init
  freya::gui_init(window);

  // Camera init
 
  freya::EntityID camera_entt = freya::entity_create(s_app.world, freya::Vec2(0.0f));

  freya::CameraDesc cam_desc = {
    .view_bounds = freya::window_get_size(window),
    .zoom        = 1.0f,
  };
  
  freya::entity_add_camera(s_app.world, camera_entt, cam_desc);
  freya::entity_add_tag(s_app.world, camera_entt, "Camera");

  // Assets init
  s_app.group_id = freya::asset_group_create("app_assets");

  // Noise generator init

  s_app.noise_entt = freya::entity_create(s_app.world, (freya::Vec2)(cam_desc.view_bounds / 2), freya::Vec2(TEXTURE_WIDTH, TEXTURE_HEIGHT));

  s_app.noise_desc = {
    .noise_type   = freya::NOISE_CELLULAR,
    .fractal_type = freya::FRACTAL_FBM,
  };
  s_app.gen = freya::entity_add_noise_generator(s_app.world, s_app.noise_entt, s_app.noise_desc);
  
  // Noise texture init
 
  sg_image_desc img_desc   = {};
  sg_sampler_desc smp_desc = {};

  img_desc.width                = TEXTURE_WIDTH;
  img_desc.height               = TEXTURE_HEIGHT;
  img_desc.pixel_format         = SG_PIXELFORMAT_RGBA32F;
  img_desc.usage.dynamic_update = true;

  // Sprite init
  freya::entity_add_sprite(s_app.world, s_app.noise_entt, freya::asset_group_push_texture(s_app.group_id, img_desc, smp_desc));

  // Generate the map 
  s_app.colors.resize(TEXTURE_WIDTH * TEXTURE_HEIGHT);

  // Done!
  return true;
}

void app_shutdown() {
  freya::entity_world_clear(s_app.world);
  freya::asset_group_destroy(s_app.group_id);

  freya::gui_shutdown();
}

void app_update(freya::f32 dt) {
  // Quit the application when the specified exit key is pressed
  
  if(freya::input_key_pressed(freya::KEY_ESCAPE)) {
    freya::event_dispatch(freya::Event{.type = freya::EVENT_APP_QUIT});
    return;
  }

  // Enable/disable debug mode

  if(freya::input_key_pressed(freya::KEY_F1)) {
    freya::gui_toggle_active();
  }

  // Re-generate the map 

  if(freya::input_key_pressed(freya::KEY_SPACE)) {
    generate_map();
  }

  // Update
  freya::entity_world_update(s_app.world, dt);
}

void app_render_gui() {
  if(!freya::gui_is_active()) {
    return;
  }

  freya::gui_begin(); 
 
  // Debug
  freya::gui_debug_info();

  // Editor
   
  freya::gui_begin_panel("Editor");
  freya::gui_edit_entity_world("Main", s_app.world);
  freya::gui_end_panel();

  freya::gui_end();
}

/// App functions 
/// ----------------------------------------------------------------------

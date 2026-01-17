#include <freya.h>

int main(int argc, char** argv) {
  // Event init
  freya::event_init();

  // Input init
  freya::input_init();

  // Window init

  int win_flags = freya::WINDOW_FLAGS_FOCUS_ON_CREATE | 
                  freya::WINDOW_FLAGS_RESIZABLE       | 
                  freya::WINDOW_FLAGS_CENTER_MOUSE;

  freya::Window* window = freya::window_open("Freya Testbed", 1600, 900, win_flags);

  // Renderer init
  
  freya::renderer_init(window); 
  freya::renderer_set_clear_color(freya::Vec4(0.1f, 0.1f, 0.1f, 1.0f));

  // Audio device init
  freya::audio_device_init(nullptr);

  // Asset manager init
  
  freya::AssetGroupID group_id = freya::asset_group_create("dungeon_alpha");
  freya::asset_group_build(group_id, "../../assets/asset_list.frlist", "dungeon_alpha.frpkg");
  freya::asset_group_load_package(group_id, "dungeon_alpha.frpkg");

  // Camera init

  freya::CameraDesc cam_desc = {
    .position = freya::Vec2(0.0f)
  };
  
  freya::Camera camera;
  freya::camera_create(camera, cam_desc);

  // Main loop

  while(freya::window_is_open(window)) {
    freya::window_poll_events(window);

    freya::input_update();
    freya::clock_update();

    freya::renderer_begin(camera);
    freya::renderer_end();
  }

  // Destroy the asset group
  freya::asset_group_destroy(group_id);

  // Shutdown the rest of the systems

  freya::audio_device_shutdown();
  freya::renderer_shutdown();
  freya::window_close(window);
  freya::event_shutdown();
}

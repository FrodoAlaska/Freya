### Project Sources ###
############################################################
set(FREYA_SOURCES
  # Core
  ${FREYA_SRC_DIR}/core/event.cpp
  ${FREYA_SRC_DIR}/core/logger.cpp
  ${FREYA_SRC_DIR}/core/memory.cpp
  ${FREYA_SRC_DIR}/core/timer.cpp
  ${FREYA_SRC_DIR}/core/clock.cpp
  ${FREYA_SRC_DIR}/core/window.cpp
  ${FREYA_SRC_DIR}/core/input.cpp
  ${FREYA_SRC_DIR}/core/engine.cpp

  # Math
  ${FREYA_SRC_DIR}/math/common.cpp
  ${FREYA_SRC_DIR}/math/random.cpp
  ${FREYA_SRC_DIR}/math/point.cpp
  ${FREYA_SRC_DIR}/math/matrix.cpp
  ${FREYA_SRC_DIR}/math/vector.cpp
  ${FREYA_SRC_DIR}/math/quaternion.cpp
  ${FREYA_SRC_DIR}/math/transform.cpp
  ${FREYA_SRC_DIR}/math/poisson_disk.cpp

  # Threads
  ${FREYA_SRC_DIR}/threads/thread_pool.cpp

  # Audio
  ${FREYA_SRC_DIR}/audio/openal_backend.cpp

  # UI
  ${FREYA_SRC_DIR}/ui/gui.cpp
  ${FREYA_SRC_DIR}/ui/rmlui_backend.cpp

  # Filesystem
  ${FREYA_SRC_DIR}/filesystem/file.cpp
  ${FREYA_SRC_DIR}/filesystem/filesystem.cpp
  ${FREYA_SRC_DIR}/filesystem/filepath.cpp
  ${FREYA_SRC_DIR}/filesystem/filewatcher.cpp

  # Assets
  ${FREYA_SRC_DIR}/freya_assets/asset_manager.cpp

  # Assets/asset_list
  ${FREYA_SRC_DIR}/freya_assets/asset_list/list.cpp
  
  # Assets/loaders
  ${FREYA_SRC_DIR}/freya_assets/loaders/texture_loader.cpp
  ${FREYA_SRC_DIR}/freya_assets/loaders/font_loader.cpp
  ${FREYA_SRC_DIR}/freya_assets/loaders/audio_loader.cpp
  ${FREYA_SRC_DIR}/freya_assets/loaders/ui_config_loader.cpp
  ${FREYA_SRC_DIR}/freya_assets/loaders/lua_state_loader.cpp
 
  # Renderer
  ${FREYA_SRC_DIR}/renderer/camera.cpp
  ${FREYA_SRC_DIR}/renderer/animation.cpp
  ${FREYA_SRC_DIR}/renderer/animator.cpp
  ${FREYA_SRC_DIR}/renderer/particles.cpp
  ${FREYA_SRC_DIR}/renderer/color.cpp
  # @TEMP: ${FREYA_SRC_DIR}/renderer/post_process.cpp
  ${FREYA_SRC_DIR}/renderer/renderer.cpp
  # @TEMP: ${FREYA_SRC_DIR}/renderer/ui_renderer.cpp
  
  # Renderer/post-process effects
  # @TEMP: ${FREYA_SRC_DIR}/renderer/post_process_effects/blur_pass.cpp
  # @TEMP: ${FREYA_SRC_DIR}/renderer/post_process_effects/greyscale_pass.cpp
  # @TEMP: ${FREYA_SRC_DIR}/renderer/post_process_effects/vignette_pass.cpp
  
  # Entity
  ${FREYA_SRC_DIR}/entity/entity.cpp
  
  # Physics
  ${FREYA_SRC_DIR}/physics/box2d_backend.cpp
  
  # Noise
  ${FREYA_SRC_DIR}/noise/noise_generator.cpp
  
  # Tile
  ${FREYA_SRC_DIR}/tile/tilemap.cpp
)

set(FREYA_HEADERS
  ${FREYA_INCLUDE_DIR}/freya.h
  ${FREYA_INCLUDE_DIR}/freya_app.h
  ${FREYA_INCLUDE_DIR}/freya_assets.h
  ${FREYA_INCLUDE_DIR}/freya_audio.h
  ${FREYA_INCLUDE_DIR}/freya_base.h
  ${FREYA_INCLUDE_DIR}/freya_containers.h
  ${FREYA_INCLUDE_DIR}/freya_entity.h
  ${FREYA_INCLUDE_DIR}/freya_event.h
  ${FREYA_INCLUDE_DIR}/freya_file.h
  ${FREYA_INCLUDE_DIR}/freya_gfx.h
  ${FREYA_INCLUDE_DIR}/freya_gui.h
  ${FREYA_INCLUDE_DIR}/freya_input.h
  ${FREYA_INCLUDE_DIR}/freya_logger.h
  ${FREYA_INCLUDE_DIR}/freya_math.h
  ${FREYA_INCLUDE_DIR}/freya_memory.h
  ${FREYA_INCLUDE_DIR}/freya_noise.h
  ${FREYA_INCLUDE_DIR}/freya_physics.h
  ${FREYA_INCLUDE_DIR}/freya_render.h
  ${FREYA_INCLUDE_DIR}/freya_threads.h
  ${FREYA_INCLUDE_DIR}/freya_tilemap.h
  ${FREYA_INCLUDE_DIR}/freya_timer.h
  ${FREYA_INCLUDE_DIR}/freya_ui.h
  ${FREYA_INCLUDE_DIR}/freya_window.h
)

set(FREYA_PCH
  ${FREYA_INCLUDE_DIR}/freya_pch.h
)
############################################################

### Library Sources ###
############################################################
set(LIBS_SOURCES 
  # glad
  ${FREYA_LIBS_DIR}/glad/glad.c

  # imgui
  
  ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
  ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp

  ${imgui_SOURCE_DIR}/misc/cpp/imgui_stdlib.cpp

  ${imgui_SOURCE_DIR}/imgui.cpp
  ${imgui_SOURCE_DIR}/imgui_demo.cpp
  ${imgui_SOURCE_DIR}/imgui_draw.cpp
  ${imgui_SOURCE_DIR}/imgui_tables.cpp
  ${imgui_SOURCE_DIR}/imgui_widgets.cpp

  # stb
  ${FREYA_LIBS_DIR}/stb/stb_image.cpp
  ${FREYA_LIBS_DIR}/stb/stb_vorbis.cpp

  # dr_libs
  ${FREYA_LIBS_DIR}/dr_libs/dr_mp3.cpp
  ${FREYA_LIBS_DIR}/dr_libs/dr_wav.cpp

  # sokol
  ${FREYA_LIBS_DIR}/sokol/sokol.c
)
############################################################

### Final Build ###
############################################################
add_library(${PROJECT_NAME} ${FREYA_BUILD_TYPE} ${FREYA_SOURCES} ${FREYA_HEADERS} ${LIBS_SOURCES})
############################################################

### Linking ###
############################################################
# @TEMP: Tracy leeks a lot of memory when turned on (??), so 
# we just turn it off for now..
# 
# if(NOT FREYA_DISTRIBUTE)
#   set(FREYA_LIBRARIES "${FREYA_LIBRARIES};TracyClient")
# endif()

target_link_options(${PROJECT_NAME} PUBLIC ${FREYA_LINKER_FLAGS})

target_include_directories(${PROJECT_NAME} PUBLIC BEFORE ${FREYA_INCLUDES})
target_link_libraries(${PROJECT_NAME} PUBLIC ${FREYA_LIBRARIES})
target_precompile_headers(${PROJECT_NAME} PRIVATE "$<$<COMPILE_LANGUAGE:CXX>:${FREYA_PCH}>")
############################################################

### Compiling Options ###
############################################################
target_compile_options(${PROJECT_NAME} PUBLIC ${FREYA_BUILD_FLAGS})
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)
target_compile_definitions(${PROJECT_NAME} PUBLIC ${FREYA_BUILD_DEFS})
############################################################

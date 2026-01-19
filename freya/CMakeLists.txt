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

  # Graphics
  ${FREYA_SRC_DIR}/graphics/gl_backend.cpp

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

  # Assets
  ${FREYA_SRC_DIR}/freya_assets/asset_manager.cpp

  # Assets/frlist
  ${FREYA_SRC_DIR}/freya_assets/frlist/frlist_lexer.cpp
  ${FREYA_SRC_DIR}/freya_assets/frlist/frlist_parser.cpp
  ${FREYA_SRC_DIR}/freya_assets/frlist/frlist.cpp
  
  # Assets/loaders
  ${FREYA_SRC_DIR}/freya_assets/loaders/texture_loader.cpp
  ${FREYA_SRC_DIR}/freya_assets/loaders/shader_loader.cpp
  ${FREYA_SRC_DIR}/freya_assets/loaders/audio_loader.cpp
  ${FREYA_SRC_DIR}/freya_assets/loaders/font_loader.cpp
 
  # Renderer
  ${FREYA_SRC_DIR}/renderer/camera.cpp
  ${FREYA_SRC_DIR}/renderer/shader_context.cpp
  ${FREYA_SRC_DIR}/renderer/animation.cpp
  ${FREYA_SRC_DIR}/renderer/particles.cpp
  ${FREYA_SRC_DIR}/renderer/renderer.cpp
  ${FREYA_SRC_DIR}/renderer/ui_renderer.cpp
  
  # Entity
  ${FREYA_SRC_DIR}/entity/entity.cpp
)

set(FREYA_HEADERS
  ${FREYA_INCLUDE_DIR}/freya.h
  ${FREYA_INCLUDE_DIR}/freya_base.h
  ${FREYA_INCLUDE_DIR}/freya_containers.h
  ${FREYA_INCLUDE_DIR}/freya_event.h
  ${FREYA_INCLUDE_DIR}/freya_logger.h
  ${FREYA_INCLUDE_DIR}/freya_memory.h
  ${FREYA_INCLUDE_DIR}/freya_window.h
  ${FREYA_INCLUDE_DIR}/freya_input.h
  ${FREYA_INCLUDE_DIR}/freya_math.h
  ${FREYA_INCLUDE_DIR}/freya_gfx.h
  ${FREYA_INCLUDE_DIR}/freya_threads.h
  ${FREYA_INCLUDE_DIR}/freya_audio.h
  ${FREYA_INCLUDE_DIR}/freya_ui.h
  ${FREYA_INCLUDE_DIR}/freya_timer.h
  ${FREYA_INCLUDE_DIR}/freya_file.h
  ${FREYA_INCLUDE_DIR}/freya_assets.h
  ${FREYA_INCLUDE_DIR}/freya_render.h
  ${FREYA_INCLUDE_DIR}/freya_app.h
  ${FREYA_INCLUDE_DIR}/freya_entity.h
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
  ${FREYA_LIBS_DIR}/imgui/backends/imgui_impl_glfw.cpp
  ${FREYA_LIBS_DIR}/imgui/backends/imgui_impl_opengl3.cpp
  ${FREYA_LIBS_DIR}/imgui/imgui.cpp
  ${FREYA_LIBS_DIR}/imgui/imgui_demo.cpp
  ${FREYA_LIBS_DIR}/imgui/imgui_draw.cpp
  ${FREYA_LIBS_DIR}/imgui/imgui_stdlib.cpp
  ${FREYA_LIBS_DIR}/imgui/imgui_stdlib.h
  ${FREYA_LIBS_DIR}/imgui/imgui_tables.cpp
  ${FREYA_LIBS_DIR}/imgui/imgui_widgets.cpp

  # stb
  ${FREYA_LIBS_DIR}/stb/stb_image.cpp
  ${FREYA_LIBS_DIR}/stb/stb_vorbis.cpp

  # dr_libs
  ${FREYA_LIBS_DIR}/dr_libs/dr_mp3.cpp
  ${FREYA_LIBS_DIR}/dr_libs/dr_wav.cpp
)
############################################################

### Final Build ###
############################################################
add_library(${PROJECT_NAME} ${FREYA_BUILD_TYPE} ${FREYA_SOURCES} ${FREYA_HEADERS} ${LIBS_SOURCES})
############################################################

### Linking ###
############################################################
target_include_directories(${PROJECT_NAME} PUBLIC BEFORE 
  ${FREYA_INCLUDES}
)

target_link_libraries(${PROJECT_NAME} PUBLIC 
  ${FREYA_LIBRARIES}
)

if(NOT FREYA_DISTRIBUTE)
  target_link_libraries(${PROJECT_NAME} PRIVATE TracyClient)
endif()

target_precompile_headers(${PROJECT_NAME} PRIVATE 
  "$<$<COMPILE_LANGUAGE:CXX>:${FREYA_PCH}>"
)
############################################################

### Compiling Options ###
############################################################
target_compile_options(${PROJECT_NAME} PUBLIC ${FREYA_BUILD_FLAGS})
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)
target_compile_definitions(${PROJECT_NAME} PUBLIC ${FREYA_BUILD_DEFS})
############################################################

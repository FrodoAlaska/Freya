### Project Sources ###
############################################################
set(FREYA_SOURCES
  # Core
  ${FREYA_SRC_DIR}/core/event.cpp
  ${FREYA_SRC_DIR}/core/logger.cpp
  ${FREYA_SRC_DIR}/core/memory.cpp
  ${FREYA_SRC_DIR}/core/timer.cpp
  ${FREYA_SRC_DIR}/core/window.cpp
  ${FREYA_SRC_DIR}/core/input.cpp

  # Math
  ${FREYA_SRC_DIR}/math/common.cpp
  ${FREYA_SRC_DIR}/math/random.cpp
  ${FREYA_SRC_DIR}/math/point.cpp
  ${FREYA_SRC_DIR}/math/matrix.cpp
  ${FREYA_SRC_DIR}/math/vector.cpp
  ${FREYA_SRC_DIR}/math/quaternion.cpp
  ${FREYA_SRC_DIR}/math/transform.cpp
  ${FREYA_SRC_DIR}/math/vertex.cpp

  # Graphics
  ${FREYA_SRC_DIR}/graphics/gl_backend.cpp

  # Threads
  ${FREYA_SRC_DIR}/threads/thread_pool.cpp
)

set(FREYA_HEADERS
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
)

set(FREYA_PCH
  ${FREYA_INCLUDE_DIR}/freya_pch.h
)
############################################################

### Library Sources ###
############################################################
set(LIBS_SOURCES 
  # GLAD
  ${FREYA_LIBS_DIR}/glad/glad.c

  # ImGui
  ${FREYA_LIBS_DIR}/imgui/backends/imgui_impl_glfw.cpp
  ${FREYA_LIBS_DIR}/imgui/backends/imgui_impl_opengl3.cpp
  ${FREYA_LIBS_DIR}/imgui/imgui.cpp
  ${FREYA_LIBS_DIR}/imgui/imgui_demo.cpp
  ${FREYA_LIBS_DIR}/imgui/imgui_draw.cpp
  ${FREYA_LIBS_DIR}/imgui/imgui_stdlib.cpp
  ${FREYA_LIBS_DIR}/imgui/imgui_stdlib.h
  ${FREYA_LIBS_DIR}/imgui/imgui_tables.cpp
  ${FREYA_LIBS_DIR}/imgui/imgui_widgets.cpp
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

cmake_minimum_required(VERSION 3.27)
project(freya)

### Project Options ###
############################################################
option(FREYA_BUILD_SHARED  "Build Freya as a shared library" OFF)
option(FREYA_BUILD_TESTBED "Build the testbeds with Freya"   ON)
option(FREYA_DISTRIBUTE    "Enable the distribution build"   OFF)

if(FREYA_BUILD_SHARED)
  set(FREYA_BUILD_TYPE SHARED)
  set(BUILD_SHARED_LIBS ON)
else()
  set(FREYA_BUILD_TYPE STATIC)
  set(BUILD_SHARED_LIBS OFF)
endif()

if(FREYA_DISTRIBUTE)
  add_definitions("-DFREYA_BUILD_DISTRIBUTION")
endif()
############################################################

### FetchContent ###
############################################################
include(FetchContent)

if(WIN32)
  if(USE_STATIC_MSVC_RUNTIME_LIBRARY)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
  endif()
endif()

# GLFW

FetchContent_Declare(
  glfw-3.4 
  URL https://github.com/glfw/glfw/releases/download/3.4/glfw-3.4.zip
)

# GLM

set(GLM_ENABLE_SIMD_AVX ON)

FetchContent_Declare(
  glm 
  URL https://github.com/g-truc/glm/archive/refs/tags/1.0.1.zip
)

# OpenAL-Soft

set(ALSOFT_EXAMPLES OFF)
set(LIBTYPE STATIC)

FetchContent_Declare(
  OpenAL 
  URL https://github.com/kcat/openal-soft/archive/refs/tags/latest.zip
)

# Tracy

FetchContent_Declare(
  tracy 
  URL https://github.com/wolfpld/tracy/archive/refs/tags/v0.12.2.zip
)

# RmlUI (fork)

FetchContent_Declare(
  rmlui 
  GIT_REPOSITORY https://github.com/FrodoAlaska/RmlUi.git
  GIT_TAG        f6875a7c13fddd23e79b378065211eaeef5f9509 
)

FetchContent_MakeAvailable(glfw-3.4 glm OpenAL tracy rmlui)
############################################################

### Project Variables ###
############################################################
set(FREYA_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/freya/src)
set(FREYA_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/freya/include)
set(FREYA_LIBS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libs)
set(FREYA_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/freya)

set(FREYA_INCLUDES 
  ${FREYA_INCLUDE_DIR}
  ${FREYA_LIBS_DIR}

  ${glfw-3.4_SOURCE_DIR}/include
  ${glm_SOURCE_DIR}
  ${OpenAL_SOURCE_DIR}/include
  ${tracy_SOURCE_DIR}/public
  ${rmlui_SOURCE_DIR}/Include
)

set(FREYA_LIBRARIES 
  glfw 
  OpenAL
  rmlui
  rmlui_debugger
)

# This script includes a bunch of platform-specific variables 
# and definitions that will be helpful later on. 
include(cmake/utils.cmake)
############################################################

### Sub Directories ###
############################################################
add_subdirectory(freya)

if(FREYA_BUILD_TESTBED) 
  add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/testbed)
endif()
############################################################

### Library Install ###
############################################################
install(TARGETS ${PROJECT_NAME} 
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib 
        ARCHIVE DESTINATION lib/${PROJECT_NAME})
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/freya/include DESTINATION include)
############################################################

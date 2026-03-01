cmake_minimum_required(VERSION 3.27)
project(freya_testbed)

### Project Variables ###
############################################################
set(TESTBED_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(TESTBED_INCLUDE_DIR ${TESTBED_SRC_DIR})
set(TESTBED_LIBS_DIR libs)

set(TESTBED_LIBRARIES 
  freya
)

set(TESTBED_INCLUDES 
  ${FREYA_INCLUDES}
  ${TESTBED_INCLUDE_DIR}
)

set(TESTBED_TYPE 
  # "engine_testbed"
  # "pathfinding_testbed"
  "noise_testbed"
)
############################################################

### Project Sources ###
############################################################
set(TESTBED_SOURCES 
  ${TESTBED_SRC_DIR}/${TESTBED_TYPE}/main.cpp
  ${TESTBED_SRC_DIR}/${TESTBED_TYPE}/app.cpp
)
############################################################

### Final Build ###
############################################################
add_executable(${PROJECT_NAME} ${TESTBED_SOURCES})
############################################################

### Linking ###
############################################################
add_dependencies(${PROJECT_NAME} freya)

target_include_directories(${PROJECT_NAME} PRIVATE BEFORE ${TESTBED_INCLUDES})
target_link_libraries(${PROJECT_NAME} PRIVATE ${TESTBED_LIBRARIES})
############################################################

### Compiling Options ###
############################################################
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)
target_compile_options(${PROJECT_NAME} PUBLIC ${FREYA_BUILD_FLAGS})
############################################################

set(FREYA_COMMON_DEFS GLFW_INCLUDE_NONE)

if(WIN32)
  ### Definitions ###
  ###############################
  set(FREYA_BUILD_DEFS 
    ${FREYA_COMMON_DEFS}
    "WIN32"
    "$<$<CONFIG:Debug>:DEBUG; _DEBUG>$<$<CONFIG:Release>:NDEBUG>"
  )
  ###############################
  
  ### Variables ###
  ###############################
  set(STATIC_LIB_EXTENSION "lib")
  set(SHARED_LIB_EXTENSION "dll")
  
  # Thanks, CMake...
  set(FREYA_LIBRARY_DIR ${FREYA_OUTPUT_DIR}/"$<$<CONFIG:Debug>:Debug>$<$<CONFIG:Release>:Release>"/freya.${STATIC_LIB_EXTENSION})
  ###############################
  
  ### Build Flags ### 
  ###############################
  set(FREYA_DEBUG_BUILD_FLAGS
    "/W0"
  )
  set(FREYA_RELEASE_BUILD_FLAGS
    "/W0 /Ot"
  )

  set(CMAKE_CXX_FLAGS_DEBUG   ${FREYA_DEBUG_BUILD_FLAGS})
  set(CMAKE_CXX_FLAGS_RELEASE ${FREYA_RELEASE_BUILD_FLAGS})
  ###############################
  
  ### Linker Flags ### 
  ###############################
  set(FREYA_LINKER_FLAGS "") 
  ###############################
elseif(LINUX) 
  ### Definitions ###
  ###############################
  set(FREYA_BUILD_DEFS 
    ${FREYA_COMMON_DEFS}
    ${BUILD_FLAGS}
  )
  ###############################
  
  ### Variables ###
  ###############################
  set(STATIC_LIB_EXTENSION "a")
  set(SHARED_LIB_EXTENSION "so")
  
  set(FREYA_LIBRARY_DIR ${FREYA_OUTPUT_DIR}/libfreyad.${STATIC_LIB_EXTENSION})
  ###############################
 
  ### Build Flags ### 
  ###############################
  set(FREYA_DEBUG_BUILD_FLAGS 
    "FREYA_BUILD_DEBUG"
    "DEBUG"
    "_DEBUG"
  )
    
  set(FREYA_RELEASE_BUILD_FLAGS 
    "FREYA_BUILD_RELEASE"
    "RELEASE"
  )

  if(CMAKE_BUILD_TYPE STREQUAL "Debug") 
    set(FREYA_BUILD_FLAGS ${FREYA_DEBUG_BUILD_FLAGS})
  else()
    set(FREYA_BUILD_FLAGS ${FREYA_RELEASE_BUILD_FLAGS})
  endif()

  set(CMAKE_CXX_FLAGS_DEBUG   ${FREYA_DEBUG_BUILD_FLAGS})
  set(CMAKE_CXX_FLAGS_RELEASE ${FREYA_RELEASE_BUILD_FLAGS})
  ###############################
  
  ### Linker Flags ### 
  ###############################
  set(FREYA_LINKER_FLAGS
    "-lm" 
    "-w"
  )
  ###############################
elseif(DEFINED EMSCRIPTEN)
  ### Definitions ###
  ###############################
  set(FREYA_BUILD_DEFS 
    ${FREYA_COMMON_DEFS}
    ${BUILD_FLAGS}
  )
  ###############################
  
  ### Variables ###
  ###############################
  set(STATIC_LIB_EXTENSION "a")
  set(SHARED_LIB_EXTENSION "so")
  
  set(FREYA_LIBRARY_DIR ${FREYA_OUTPUT_DIR}/libfreyad.${STATIC_LIB_EXTENSION})
  ###############################
 
  ### Build Flags ### 
  ###############################
  set(FREYA_DEBUG_BUILD_FLAGS
    "--use-port=contrib.glfw3"
  )

  set(FREYA_RELEASE_BUILD_FLAGS
    "-Wall"
    "--use-port=contrib.glfw3"
    "-Os"
  )

  if(CMAKE_BUILD_TYPE STREQUAL "Debug") 
    set(FREYA_BUILD_FLAGS ${FREYA_DEBUG_BUILD_FLAGS})
  else()
    set(FREYA_BUILD_FLAGS ${FREYA_RELEASE_BUILD_FLAGS})
  endif()
  
  set(CMAKE_CXX_FLAGS_DEBUG   ${FREYA_DEBUG_BUILD_FLAGS})
  set(CMAKE_CXX_FLAGS_RELEASE ${FREYA_RELEASE_BUILD_FLAGS})
  ###############################
  
  ### Linker Flags ### 
  ###############################
  set(FREYA_LINKER_FLAGS
    "-sUSE_GLFW=3"
    "-sFORCE_FILESYSTEM=1"
    "-sFULL_ES3=1"
  )
  ###############################
endif()

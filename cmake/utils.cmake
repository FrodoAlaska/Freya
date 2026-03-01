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
elseif(LINUX) 
  if(CMAKE_BUILD_TYPE STREQUAL "Debug") 
    set(BUILD_FLAGS 
      "FREYA_BUILD_DEBUG"
      "DEBUG"
      "_DEBUG"
    )
  else()
    set(BUILD_FLAGS 
      "FREYA_BUILD_RELEASE"
      "RELEASE"
    )
  endif()

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
  set(FREYA_BUILD_FLAGS
    "-lm" 
    "-w"
  )

  # TODO (Linux build)
  set(CMAKE_CXX_FLAGS_DEBUG   ${FREYA_BUILD_FLAGS})
  set(CMAKE_CXX_FLAGS_RELEASE ${FREYA_BUILD_FLAGS})
  ###############################
elseif(WEB) # Must be the web, right?
  if(CMAKE_BUILD_TYPE STREQUAL "Debug") 
    set(BUILD_FLAGS 
      "FREYA_BUILD_DEBUG"
      "DEBUG"
      "_DEBUG"
    )
  else()
    set(BUILD_FLAGS 
      "FREYA_BUILD_RELEASE"
      "RELEASE"
    )
  endif()

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
    "-sUSE_GLFW=3"
    "-sFORCE_FILESYSTEM=1"
    "-sFULL_ES3=1"
  )
  
  set(FREYA_RELEASE_BUILD_FLAGS
    "-Wall"
    "-sUSE_GLFW=3"
    "-sFORCE_FILESYSTEM=1"
    "-sFULL_ES3=1"
    "-Os"
  )
  
  set(CMAKE_CXX_FLAGS_DEBUG   ${FREYA_DEBUG_BUILD_FLAGS})
  set(CMAKE_CXX_FLAGS_RELEASE ${FREYA_RELEASE_BUILD_FLAGS})
  ###############################
endif()

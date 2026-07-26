#define SOKOL_IMPL

#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32) || defined(__linux__) || defined(__gnu_linux__)
  #define SOKOL_GLCORE
#elif defined(__EMSCRIPTEN__)
  #define SOKOL_GLES3 
#endif

#if defined(DEBUG) || defined(_DEBUG)
  #define SOKOL_DEBUG
#endif

#include "sokol_gfx.h"
#include "sokol_gl.h"
#include "sokol_gp.h"

#define FONTSTASH_IMPLEMENTATION

#include "fontstash/fontstash.h"
#include "sokol_fontstash.h"

#define CLAY_IMPLEMENTATION
#include "clay/clay.h"

#define SOKOL_CLAY_NO_SOKOL_APP
#define SOKOL_CLAY_IMPL
#include "sokol_clay.h"

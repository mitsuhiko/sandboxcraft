/* bootstraps sandboxcraft */
#ifndef _INC_SC_BOOT_H_
#define _INC_SC_BOOT_H_

/* platform identifiers */
#define SC_PLATFORM_WINDOWS 1
#define SC_PLATFORM_OSX     2
#define SC_PLATFORM_LINUX   3

/* figure out the current platform and include whatever's necessary */
#if defined(_WIN32) || defined(_WINDOWS)
#  define _CRT_SECURE_NO_WARNINGS
#  define WIN32_LEAN_AND_MEAN
#  define NOMINMAX
#  include <windows.h>
#  include <fcntl.h>
#  include <io.h>
#  include <conio.h>
#  define SC_PLATFORM SC_PLATFORM_WINDOWS
#elif defined(__APPLE__) && defined(__MACH__)
#  define SC_PLATFORM SC_PLATFORM_OSX
#elif defined(__linux)
#  define _GNU_SOURCE
#  define SC_PLATFORM SC_PLATFORM_LINUX
#else
#  error "Unsupported platform"
#endif

/* handy little helper to trigger a segfault */
#define SC_SEGFAULT { int *x = 0; *x = 42; }

/* SDL includes */
#include "SDL.h"
#include "SDL_opengl.h"
#include "SDL_image.h"

#if SC_PLATFORM == SC_PLATFORM_OSX
#  include <GLUT/glut.h>
#else
#  include <glut.h>
#endif

/* do not use the main macro.  We take care of that ourselves */
#ifdef main
#  undef main
#endif

/* make sure we have the error system and our allocators */
#include "sc_error.h"
#include "sc_alloc.h"

#endif

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
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

/* what does the compiler use to mark functions as inline? */
#ifdef _MSC_VER
#  define sc_inline __inline
#else
#  define sc_inline inline
#endif

/* what can we use for thread local storage?  Note that on OS X this is
   currently not supported at all, so we can't use it yet */
#ifdef _MSC_VER
#  define sc_threadlocal __declspec(thread)
#else
#  define sc_threadlocal __thread
#endif
#if SC_PLATFORM != SC_PLATFORM_OSX
#  define SC_THREADLOCAL_SUPPORTED
#endif

/* msvc does not have a ssize_t.  Add one */
#ifdef _MSC_VER
typedef signed long ssize_t;
#endif

/* handy little helper to trigger a segfault */
#define SC_SEGFAULT do { int *x = 0; *x = 42; } while (0)

/* GLee */
#include "GLee.h"

/* SDL includes */
#include "SDL.h"
#include "SDL_opengl.h"
#include "SDL_image.h"

/* do not use the main macro.  We take care of that ourselves */
#ifdef main
#  undef main
#endif

/* make sure we have the error system and our allocators */
#include "sc_config.h"
#include "sc_error.h"
#include "sc_alloc.h"

#endif

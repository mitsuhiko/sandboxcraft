cmake_minimum_required(VERSION 2.6)

project(SandboxCraft)

set(SOURCES
	src/main.c
    src/alloc.c
    src/error.c
    src/engine.c
    src/game.c
    src/path.c
    src/texture.c
)
set(HEADERS
	include/sc_boot.h
    include/sc_alloc.h
    include/sc_engine.h
    include/sc_error.h
    include/sc_stringutils.h
    include/sc_path.h
    include/sc_engine.h
    include/sc_game.h
    include/sc_texture.h
)

find_package(SDL 1.2.7 REQUIRED)
find_package(SDL_Image 1.2 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)

if (APPLE)
	set(SOURCES ${SOURCES} osxlauncher/launcher.m)
	set(HEADERS ${HEADERS} osxlauncher/launcher.h)
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include ${SDL_INCLUDE_DIR})

add_executable(
	SandboxCraft
	WIN32
	MACOSX_BUNDLE
	${SOURCES}
	${HEADERS}
)

target_link_libraries(
	SandboxCraft
	${SDL_LIBRARY}
	${SDLIMAGE_LIBRARY}
	${OPENGL_LIBRARIES}
	${GLU_LIBRARY}
	${GLUT_LIBRARY}
)

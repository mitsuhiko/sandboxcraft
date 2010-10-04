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
    src/blocks.c
    src/world.c
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
    include/sc_blocks.h
    include/sc_world.h
)

find_package(SDL 1.2.7 REQUIRED)
find_package(SDL_Image 1.2 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)

if (APPLE)
	set(SOURCES ${SOURCES} osxlauncher/launcher.m)
	set(HEADERS ${HEADERS} osxlauncher/launcher.h)
endif()
if (NOT WIN32)
    set(CMAKE_C_FLAGS "-g -Wall")
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include ${SDL_INCLUDE_DIR} ${SDLIMAGE_INCLUDE_DIR})

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

if (WIN32)
    set(VS_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR})
    add_custom_command(TARGET SandboxCraft POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${SDL_INCLUDE_DIR}/../lib/SDL.dll" "${VS_OUTPUT_DIR}/SDL.dll"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${SDLIMAGE_INCLUDE_DIR}/../lib/SDL_image.dll" "${VS_OUTPUT_DIR}/SDL_image.dll"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${SDLIMAGE_INCLUDE_DIR}/../lib/libpng12-0.dll" "${VS_OUTPUT_DIR}/libpng12-0.dll"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${SDLIMAGE_INCLUDE_DIR}/../lib/jpeg.dll" "${VS_OUTPUT_DIR}/jpeg.dll"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${SDLIMAGE_INCLUDE_DIR}/../lib/zlib1.dll" "${VS_OUTPUT_DIR}/zlib1.dll"
    )
endif()

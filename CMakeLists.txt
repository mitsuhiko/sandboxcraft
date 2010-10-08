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
    src/atlas.c
    src/blocks.c
    src/world.c
    src/vec3.c
    src/vec4.c
    src/mat3.c
    src/mat4.c
    src/camera.c
    src/math.c
    src/color.c
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
    include/sc_atlas.h
    include/sc_blocks.h
    include/sc_world.h
    include/sc_vec3.h
    include/sc_vec4.h
    include/sc_mat3.h
    include/sc_mat4.h
    include/sc_camera.h
    include/sc_math.h
    include/sc_color.h
)

find_package(SDL 1.2.7 REQUIRED)
find_package(SDL_image 1.2 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)

if (APPLE)
	set(SOURCES ${SOURCES} osxlauncher/launcher.m)
	set(HEADERS ${HEADERS} osxlauncher/launcher.h)
endif()
if (NOT WIN32)
    set(CMAKE_C_FLAGS "-g -Wall")
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include ${SDL_INCLUDE_DIR} ${SDLIMAGE_INCLUDE_DIR} ${GLUT_INCLUDE_DIR})

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
	${GLUT_glut_LIBRARY}
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

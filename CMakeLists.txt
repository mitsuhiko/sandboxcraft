cmake_minimum_required(VERSION 2.6)

project(SandboxCraft)

# do not add the test sources here.  Also not the stuff from test.c in the
# src folder!

set(LIB_SOURCES
    src/alloc.c
    src/error.c
    src/utils.c
    src/engine.c
    src/game.c
    src/path.c
    src/texture.c
    src/atlas.c
    src/blocks.c
    src/world.c
    src/vec2.c
    src/vec3.c
    src/vec4.c
    src/mat3.c
    src/mat4.c
    src/camera.c
    src/math.c
    src/color.c
    src/frustum.c
    src/vbo.c
    src/primitives.c
    src/ray.c
    src/shading.c
    src/debug.c
    src/rnd.c
    src/perlin.c
    src/worldgen.c
    src/list.c
    src/_list_impl.inc
    src/threads.c

    deps/GLee/GLee.c
)
set(LIB_HEADERS
	include/sc_boot.h
    include/sc_alloc.h
    include/sc_engine.h
    include/sc_error.h
    include/sc_utils.h
    include/sc_path.h
    include/sc_engine.h
    include/sc_game.h
    include/sc_texture.h
    include/sc_atlas.h
    include/sc_blocks.h
    include/sc_world.h
    include/sc_vec2.h
    include/sc_vec3.h
    include/sc_vec4.h
    include/sc_mat3.h
    include/sc_mat4.h
    include/sc_camera.h
    include/sc_math.h
    include/sc_color.h
    include/sc_frustum.h
    include/sc_vbo.h
    include/sc_primitives.h
    include/sc_ray.h
    include/sc_shading.h
    include/sc_debug.h
    include/sc_rnd.h
    include/sc_perlin.h
    include/sc_worldgen.h
    include/sc_list.h
    include/_sc_list.h
    include/_sc_list_pre.inc
    include/_sc_list_post.inc
    include/sc_threads.h

    deps/GLee/GLee.h
)

set(GAME_SOURCES
    src/main.c
)
set(GAME_HEADERS
)

if (APPLE)
	set(GAME_SOURCES ${GAME_SOURCES} osxlauncher/launcher.m)
	set(GAME_HEADERS ${GAME_HEADERS} osxlauncher/launcher.h)
endif()

find_package(SDL 1.2.7 REQUIRED)
find_package(SDL_image 1.2 REQUIRED)
find_package(OpenGL REQUIRED)

if (NOT WIN32)
    set(CMAKE_C_FLAGS "-g -Wall")
endif()

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${SDL_INCLUDE_DIR}
    ${SDLIMAGE_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/deps/GLee
)

add_executable(
	SandboxCraft
	WIN32
	MACOSX_BUNDLE
    ${GAME_SOURCES}
    ${GAME_HEADERS}
)

add_library(
    libsandboxcraft
    STATIC
	${LIB_SOURCES}
	${LIB_HEADERS}
)

target_link_libraries(
	SandboxCraft
    libsandboxcraft
	${SDL_LIBRARY}
	${SDLIMAGE_LIBRARY}
	${OPENGL_LIBRARIES}
	${GLU_LIBRARY}
)

add_subdirectory(tests)

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

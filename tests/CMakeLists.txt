set(TESTS
    test_vec3
)

foreach(TEST ${TESTS})
    add_executable(${TEST}.bin ${TEST}.c ../src/test.c)
    target_link_libraries(${TEST}.bin
        libsandboxcraft
        ${SDL_LIBRARY}
        ${SDLIMAGE_LIBRARY}
        ${OPENGL_LIBRARIES}
        ${GLU_LIBRARY}
        ${GLUT_glut_LIBRARY}
    )
endforeach()

add_custom_target(test
    ${CMAKE_CURRENT_BINARY_DIR}/runtests.sh
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

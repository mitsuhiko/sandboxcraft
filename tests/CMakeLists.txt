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
    set(TEST_EXECUTABLES ${TEST}.bin ${TEST_EXECUTABLES})
endforeach()

add_custom_target(test
    python ${CMAKE_CURRENT_BINARY_DIR}/runtests.py
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    DEPENDS ${TEST_EXECUTABLES}
)

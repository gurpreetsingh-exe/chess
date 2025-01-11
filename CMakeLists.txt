cmake_minimum_required(VERSION 3.26)
project(chess VERSION 0.1)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_C_STANDARD 23)
set(CMAKE_CXX_STANDARD 23)

set(LIBDIR ${CMAKE_SOURCE_DIR}/lib)
set(THIRD_PARTY "${CMAKE_SOURCE_DIR}/third-party")

add_library(chess
    ${LIBDIR}/bitboard.c
    ${LIBDIR}/board.c
    ${LIBDIR}/engine.c
    ${LIBDIR}/move.c
    ${LIBDIR}/movegen.c
    ${LIBDIR}/perft.c
    ${LIBDIR}/types.c)

target_compile_options(chess PRIVATE
    -ggdb3
    -Wundef
    -Wcast-align
    -Wsign-conversion
    -Wformat=2
    -Wconversion
    -Wshadow

    -Werror=nonnull
    -Werror=address
    -Werror=init-self
    -Werror=uninitialized
    -Werror=return-type
    -Werror=pointer-arith
    -Werror=implicit-fallthrough
    -Werror=missing-include-dirs
    -Werror=string-compare
    -Werror=switch
    -Werror=switch-enum
    -Werror=write-strings

    -Werror=missing-field-initializers

    $<$<CONFIG:Debug>:-fsanitize=address>
)

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(chess PRIVATE
        -Wlogical-op
        -Werror=invalid-memory-model
        -Werror=maybe-uninitialized
        # -Werror=missing-requires
        -Werror=return-local-addr)
endif()

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_options(chess PRIVATE
        -Werror=dangling
        -Werror=return-stack-address)
endif()

target_precompile_headers(chess PRIVATE "${CMAKE_SOURCE_DIR}/include/base.h")
target_link_options(chess PRIVATE $<$<CONFIG:Debug>:-fsanitize=address>)
target_include_directories(chess PUBLIC "${CMAKE_SOURCE_DIR}/include")

####################################################################
########################## Engine ##################################
####################################################################
add_executable(engine
    ${LIBDIR}/main.c
    ${LIBDIR}/command.c
    ${LIBDIR}/repl.c)
target_precompile_headers(engine PRIVATE "${CMAKE_SOURCE_DIR}/include/base.h")
target_link_options(engine PRIVATE $<$<CONFIG:Debug>:-fsanitize=address>)
target_link_libraries(engine chess)

####################################################################
########################## Tests ###################################
####################################################################
add_executable(tests "${LIBDIR}/tests.c")
target_precompile_headers(tests PRIVATE "${CMAKE_SOURCE_DIR}/include/base.h")
target_link_options(tests PRIVATE $<$<CONFIG:Debug>:-fsanitize=address>)
target_link_libraries(tests chess)

####################################################################
############################ GUI ###################################
####################################################################
add_executable(gui "${LIBDIR}/gui.c" ${THIRD_PARTY}/stbi/stb_image.c)
target_compile_options(gui PRIVATE -ggdb3)
target_include_directories(gui PRIVATE ${THIRD_PARTY}/stbi)
target_precompile_headers(gui PRIVATE "${CMAKE_SOURCE_DIR}/include/base.h")
target_link_options(gui PRIVATE $<$<CONFIG:Debug>:-fsanitize=address>)
find_package(OpenGL REQUIRED)
target_link_libraries(gui chess glfw OpenGL::GL GLEW m)

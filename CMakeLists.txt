cmake_minimum_required(VERSION 3.26)
project(chess VERSION 0.1)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_C_STANDARD 23)
set(CMAKE_CXX_STANDARD 23)

set(GUI_SOURCE "${CMAKE_SOURCE_DIR}/lib/gui.c")

file(GLOB_RECURSE sources "${CMAKE_SOURCE_DIR}/lib/*.c")
list(REMOVE_ITEM sources ${GUI_SOURCE})

add_library(chess ${sources})
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

set(THIRD_PARTY "${CMAKE_SOURCE_DIR}/third-party")

add_executable(gui ${GUI_SOURCE} ${THIRD_PARTY}/stbi/stb_image.c)
target_compile_options(gui PRIVATE -ggdb3)
target_include_directories(gui PRIVATE ${THIRD_PARTY}/stbi)
target_precompile_headers(gui PRIVATE "${CMAKE_SOURCE_DIR}/include/base.h")
target_link_options(gui PRIVATE $<$<CONFIG:Debug>:-fsanitize=address>)
find_package(OpenGL REQUIRED)
target_link_libraries(gui chess glfw OpenGL::GL GLEW m)

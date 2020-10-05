cmake_minimum_required(VERSION 3.0)
project(opengl-imgui-sample CXX)

set(CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR})
set(CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR})
SET(CMAKE_CXX_FLAGS "-std=c++11")


# CONFIG option is important so that CMake doesnt search for modules into the default modules directory
find_package(imgui CONFIG)
find_package(glfw CONFIG)
find_package(glew CONFIG)
find_package(fmt CONFIG)
find_package(glm CONFIG)

add_executable( mandelbrot_run
                main.cpp
                opengl_shader.cpp
                opengl_shader.h
                bindings/imgui_impl_glfw.cpp
                bindings/imgui_impl_opengl3.cpp
                bindings/imgui_impl_glfw.h
                bindings/imgui_impl_opengl3.h
                assets/simple-shader.vs
                assets/simple-shader.fs )

add_custom_command(TARGET mandelbrot_run
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/assets/simple-shader.vs ${PROJECT_BINARY_DIR}
    COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/assets/simple-shader.fs ${PROJECT_BINARY_DIR}
)

target_compile_definitions(mandelbrot_run PUBLIC IMGUI_IMPL_OPENGL_LOADER_GLEW)
target_link_libraries(mandelbrot_run imgui::imgui GLEW::glew_s glfw::glfw fmt::fmt glm::glm)

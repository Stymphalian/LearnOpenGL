cmake_minimum_required(VERSION 3.10.2)
include(GNUInstallDirs)

set(CMAKE_C_COMPILER "/usr/bin/clang")
set(CMAKE_CXX_COMPILER "/usr/bin/clang++")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")
enable_testing()

project(gl)

# Libraries
find_package(GLFW3 REQUIRED)
find_package(glm REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(ASSIMP REQUIRED)

# IMGUI
add_library(imgui STATIC "")
target_sources(imgui PUBLIC
  "${CMAKE_CURRENT_SOURCE_DIR}/third_party/imgui/imgui.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/third_party/imgui/imgui_draw.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/third_party/imgui/imgui_tables.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/third_party/imgui/imgui_widgets.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/third_party/imgui/backends/imgui_impl_glfw.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/third_party/imgui/backends/imgui_impl_opengl3.cpp"
)
target_include_directories(imgui
  PUBLIC
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/third_party/imgui>"
)
target_include_directories(imgui
  PUBLIC
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/third_party/imgui/backends>"
)

# Executable
add_executable(noin "")
target_sources(noin PUBLIC 
  "src/main.cpp"
  "src/hand_mesh.cpp"
  "src/object.cpp"
  "src/misc.cpp"
  "src/camera.cpp"
  "src/mesh.cpp"
  "src/point.cpp"
  "src/cloth.cpp"
  "src/stb_image.cpp"
  "src/time.cpp"
  "src/light.cpp"
  "src/model.cpp")
target_link_libraries(noin LINK_PUBLIC GLEW)
target_link_libraries(noin LINK_PUBLIC GL)
target_link_libraries(noin LINK_PUBLIC glfw)
target_link_libraries(noin LINK_PUBLIC rt)
target_link_libraries(noin LINK_PUBLIC m)
target_link_libraries(noin LINK_PUBLIC dl)
target_link_libraries(noin LINK_PUBLIC assimp)
target_link_libraries(noin LINK_PUBLIC imgui)
target_link_libraries(noin LINK_PUBLIC stdc++fs)

# Resources
add_custom_command(
    TARGET noin POST_BUILD
    COMMAND 
      ln -s ${CMAKE_CURRENT_SOURCE_DIR}/res ${CMAKE_CURRENT_BINARY_DIR}/res)
add_custom_command(
    TARGET noin POST_BUILD
    COMMAND 
      ln -s ${CMAKE_CURRENT_SOURCE_DIR}/shaders ${CMAKE_CURRENT_BINARY_DIR}/shaders)



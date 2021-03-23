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
add_subdirectory(third_party/abseil-cpp)
add_subdirectory(third_party/googletest)

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

# Executable Library
add_library(noin_lib STATIC "")
target_sources(noin_lib PUBLIC 
  "${CMAKE_CURRENT_SOURCE_DIR}/src/hand_mesh.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/object.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/misc.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/camera.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/mesh.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/point.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/cloth.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/stb_image.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/time.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/light.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/model.cpp")
target_link_libraries(noin_lib LINK_PUBLIC GLEW)
target_link_libraries(noin_lib LINK_PUBLIC GL)
target_link_libraries(noin_lib LINK_PUBLIC glfw)
target_link_libraries(noin_lib LINK_PUBLIC rt)
target_link_libraries(noin_lib LINK_PUBLIC m)
target_link_libraries(noin_lib LINK_PUBLIC dl)
target_link_libraries(noin_lib LINK_PUBLIC assimp)
target_link_libraries(noin_lib LINK_PUBLIC imgui)
target_link_libraries(noin_lib LINK_PUBLIC stdc++fs)
target_link_libraries(noin_lib LINK_PUBLIC absl::strings)
target_link_libraries(noin_lib LINK_PUBLIC absl::str_format)

# Executable
add_executable(noin "")
target_sources(noin PUBLIC "src/main.cpp")
target_link_libraries(noin LINK_PUBLIC noin_lib)

# Resources
add_custom_command(
  TARGET noin POST_BUILD
  COMMAND ln -snf "${CMAKE_SOURCE_DIR}/res" "${CMAKE_CURRENT_BINARY_DIR}/res"
  COMMAND ln -snf "${CMAKE_SOURCE_DIR}/shaders" "${CMAKE_CURRENT_BINARY_DIR}/shaders"
)

# Tests
add_executable(camera_test "")
target_sources(camera_test PRIVATE "src/camera_test.cpp")
add_test(NAME camera_test COMMAND camera_test)
target_link_libraries(camera_test PRIVATE noin_lib)
target_link_libraries(camera_test PRIVATE gtest)



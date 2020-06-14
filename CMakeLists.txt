cmake_minimum_required(VERSION 3.12)

set(common_files common/shader.cc src/glad.c)

add_executable(hello_window hello_window/hello_window.cc ${common_files})
target_include_directories(hello_window PRIVATE ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/common)
target_link_libraries(hello_window LINK_PUBLIC glfw GL dl)

add_executable(color_triangle color_triangle/color_triangle.cc ${common_files})
target_include_directories(color_triangle PRIVATE ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/common)
target_link_libraries(color_triangle LINK_PUBLIC glfw GL dl m)

add_executable(texture texture/texture.cc ${common_files})
target_include_directories(texture PRIVATE ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/common)
target_link_libraries(texture LINK_PUBLIC glfw GL dl m)

add_executable(transformation transformation/transformation.cc ${common_files})
target_include_directories(transformation PRIVATE ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/common)
target_link_libraries(transformation LINK_PUBLIC glfw GL dl m)

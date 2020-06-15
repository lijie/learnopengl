cmake_minimum_required(VERSION 3.12)
project(learnopengl)

set(common_files common/shader.cc src/glad.c)

set(dep_libs_win opengl32 legacy_stdio_definitions)
set(dep_libs_linux GL dl m)

include_directories(${CMAKE_SOURCE_DIR}/deps/glm-0.9.9.9)

if (WIN32)
	ADD_LIBRARY(glfw STATIC IMPORTED)
	SET_TARGET_PROPERTIES(glfw PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/deps/glfw-3.3.2.bin.WIN64/lib-vc2019/glfw3.lib)
	include_directories(${CMAKE_SOURCE_DIR}/deps/glfw-3.3.2.bin.WIN64/include)
	set(dep_libs ${dep_libs_win})
else ()
	set(dep_libs ${dep_libs_linux})
endif (WIN32)

add_executable(hello_window hello_window/hello_window.cc ${common_files})
target_include_directories(hello_window PRIVATE ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/common)
target_link_libraries(hello_window LINK_PUBLIC glfw ${dep_libs})

add_executable(color_triangle color_triangle/color_triangle.cc ${common_files})
target_include_directories(color_triangle PRIVATE ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/common)
target_link_libraries(color_triangle LINK_PUBLIC glfw ${dep_libs})

add_executable(texture texture/texture.cc ${common_files})
target_include_directories(texture PRIVATE ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/common)
target_link_libraries(texture LINK_PUBLIC glfw ${dep_libs})

add_executable(transformation transformation/transformation.cc ${common_files})
target_include_directories(transformation PRIVATE ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/common)
target_link_libraries(transformation LINK_PUBLIC glfw ${dep_libs})

add_executable(coordinate coordinate/coordinate.cc ${common_files})
target_include_directories(coordinate PRIVATE ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/common)
target_link_libraries(coordinate LINK_PUBLIC glfw ${dep_libs})

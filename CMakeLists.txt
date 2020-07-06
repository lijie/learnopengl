cmake_minimum_required(VERSION 3.12)
project(learnopengl)

set(dep_libs_win opengl32 legacy_stdio_definitions)
set(dep_libs_linux GL dl m assimp)

include_directories(${CMAKE_SOURCE_DIR}/deps/glm-0.9.9.9)
include_directories(${CMAKE_SOURCE_DIR}/deps/assimp/include)

if (WIN32)
	ADD_LIBRARY(glfw STATIC IMPORTED)
	SET_TARGET_PROPERTIES(glfw PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/deps/glfw-3.3.2.bin.WIN64/lib-vc2019/glfw3.lib)
	include_directories(${CMAKE_SOURCE_DIR}/deps/glfw-3.3.2.bin.WIN64/include)

	ADD_LIBRARY(assimp STATIC IMPORTED)
	SET_TARGET_PROPERTIES(assimp PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/deps/assimp/lib/assimp-vc142-mtd.lib)

	set(dep_libs ${dep_libs_win} assimp)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /utf-8 /std:c++17")
else ()
  set(dep_libs ${dep_libs_linux})
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
endif (WIN32)

add_library(toy common/shader.cc common/camera.cc src/glad.c common/lo_common.cc common/shape.cc common/material.cc common/model.cc common/mesh.cc common/scene.cc)
target_include_directories(toy PRIVATE ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/common)

add_executable(hello_window hello_window/hello_window.cc ${common_files})
target_include_directories(hello_window PRIVATE ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/common)
target_link_libraries(hello_window LINK_PUBLIC toy glfw ${dep_libs})

add_executable(color_triangle color_triangle/color_triangle.cc ${common_files})
target_include_directories(color_triangle PRIVATE ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/common)
target_link_libraries(color_triangle LINK_PUBLIC toy glfw ${dep_libs})

add_executable(texture texture/texture.cc ${common_files})
target_include_directories(texture PRIVATE ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/common)
target_link_libraries(texture LINK_PUBLIC toy glfw ${dep_libs})

add_executable(transformation transformation/transformation.cc ${common_files})
target_include_directories(transformation PRIVATE ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/common)
target_link_libraries(transformation LINK_PUBLIC toy glfw ${dep_libs})

add_executable(coordinate coordinate/coordinate.cc ${common_files})
target_include_directories(coordinate PRIVATE ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/common)
target_link_libraries(coordinate LINK_PUBLIC toy glfw ${dep_libs})

add_executable(camera camera/camera.cc ${common_files})
target_include_directories(camera PRIVATE ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/common)
target_link_libraries(camera LINK_PUBLIC toy glfw ${dep_libs})

add_executable(basic_lighting basic_lighting/basic_lighting.cc ${common_files})
target_include_directories(basic_lighting PRIVATE ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/common)
target_link_libraries(basic_lighting LINK_PUBLIC toy glfw ${dep_libs})

add_executable(basic_model basic_model/basic_model.cc ${common_files})
target_include_directories(basic_model PRIVATE ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/common)
target_link_libraries(basic_model LINK_PUBLIC toy glfw ${dep_libs})

add_executable(stencil_test stencil_test/stencil_test.cc ${common_files})
target_include_directories(stencil_test PRIVATE ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/common)
target_link_libraries(stencil_test LINK_PUBLIC toy glfw ${dep_libs})

add_executable(blending_test blending_test/blending_test.cc ${common_files})
target_include_directories(blending_test PRIVATE ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/common)
target_link_libraries(blending_test LINK_PUBLIC toy glfw ${dep_libs})

add_executable(framebuffer_test framebuffer_test/framebuffer_test.cc ${common_files})
target_include_directories(framebuffer_test PRIVATE ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/common)
target_link_libraries(framebuffer_test LINK_PUBLIC toy glfw ${dep_libs})

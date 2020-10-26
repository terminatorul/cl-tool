cmake_minimum_required(VERSION 3.2)

message("-- Updating git submodules")
execute_process (COMMAND git submodule update --init --recursive WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
if(NOT EXISTS "${CMAKE_SOURCE_DIR}/OpenCL-Headers/CL/cl2.hpp")
    file(DOWNLOAD
            "https://github.com/KhronosGroup/OpenCL-CLHPP/releases/download/v2.0.10/cl2.hpp"
            "${CMAKE_SOURCE_DIR}/OpenCL-Headers/CL/cl2.hpp"
            SHOW_PROGRESS)
endif()
set(OPENCL_INCLUDE_DIRS "${CMAKE_SOURCE_DIR}/OpenCL-Headers")

add_definitions("-DCL_TARGET_OPENCL_VERSION=220")
add_subdirectory("OpenCL-ICD-Loader")

project(cl-tool CXX)

# include(UseOpenCL.cmake)

if (NOT WIN32)
    include(UseGoogleTest.cmake)
endif()

find_package(Threads)

set(OPENCL_INCLUDE_DIRS "${CMAKE_SOURCE_DIR}/OpenCL-Headers")
set(OPENCL2_HPP_INCLUDE_DIRS "${CMAKE_SOURCE_DIR}/OpenCL-Headers")
set(OPENCL_LIBRARIES OpenCL)

set(CL_TOOL_SOURCES
	parse-cmd-line.hh
	parse-cmd-line.cc
	cl-matrix-mult.hh
	cl-matrix-mult.cc
	cl-double-pendulum.hh
	cl-double-pendulum.cc
	cl-platform-info.hh
	cl-platform-info.cc
	cl-platform-probe.hh
	cl-platform-probe.cc
	cl-user-selection.hh
	cl-user-selection.cc
	cl-tool.cc)
set(CL_TOOL_TARGET_SOURCES cl-matrix-rand.cl cl-double-pendulum.cl)

add_executable(cl-tool ${CL_TOOL_SOURCES})
target_compile_features(cl-tool PRIVATE cxx_std_14)
target_compile_definitions(cl-tool PRIVATE CL_HPP_TARGET_OPENCL_VERSION=200 CL_HPP_MINIMUM_OPENCL_VERSION=110 CL_HPP_CL_1_2_DEFAULT_BUILD CL_HPP_ENABLE_EXCEPTIONS)
target_compile_definitions(cl-tool PRIVATE __CL_ENABLE_EXCEPTIONS CL_VERSION_1_2)
target_include_directories(cl-tool PRIVATE ${OPENCL_INCLUDE_DIRS} ${OPENCL2_HPP_INCLUDE_DIRS})

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    target_compile_options(cl-tool PRIVATE -Wno-ignored-attributes -fvisibility=hidden)

    set(CMAKE_EXE_LINKER_FLAGS ${CMAKE_EXE_LINKER_FLAGS} -Wl,--allow-multiple-definition)
    if (WIN32)
	    target_link_libraries(cl-tool "-Wl,--allow-multiple-definition")
    endif()
endif()

target_link_libraries(cl-tool ${OPENCL_LIBRARIES} Threads::Threads)

if (WIN32)
    foreach(TARGET_SRC ${CL_TOOL_TARGET_SOURCES})
	configure_file("${TARGET_SRC}" "${TARGET_SRC}" COPYONLY)
    endforeach()
else()
    add_custom_target(cl-matrix-rand.cl DEPENDS "${PROJECT_SOURCE_DIR}/cl-matrix-rand.cl" BYPRODUCTS cl-matrix-rand.cl WORKING DIRECTORY . COMMAND "${CMAKE_COMMAND}" -E create_symlink "${PROJECT_SOURCE_DIR}/cl-matrix-rand.cl" "cl-matrix-rand.cl")
    add_dependencies(cl-tool cl-matrix-rand.cl)
    add_custom_target(cl-double-pendulum.cl DEPENDS "${PROJECT_SOURCE_DIR}/cl-double-pendulum.cl" BYPRODUCTS cl-double-pendulum.cl WORKING DIRECTORY . COMMAND "${CMAKE_COMMAND}" -E create_symlink "${PROJECT_SOURCE_DIR}/cl-double-pendulum.cl" "cl-double-pendulum.cl")
    add_dependencies(cl-tool cl-double-pendulum.cl)
endif()

add_custom_target(tags DEPENDS ${CL_TOOL_SOURCES} BYPRODUCTS tags WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    COMMAND echo Building tags file...
    COMMAND ctags -R ${CL_TOOL_SOURCES} ${OPENCL_INCLUDE_DIRS} ${OPENCL2_HPP_INCLUDE_DIRS})
# add_dependencies(cl-tool tags)

if (NOT WIN32)
    enable_testing()
    add_executable(cl-tool-unit-tests cl-platform-info.hh cl-platform-info.cc unit-tests/cl-tool-unit-test.cc)
    target_compile_features(cl-tool-unit-tests PRIVATE cxx_std_14)
    target_compile_definitions(cl-tool-unit-tests PRIVATE CL_HPP_TARGET_OPENCL_VERSION=120 CL_HPP_MINIMUM_OPENCL_VERSION=110 CL_HPP_CL_1_2_DEFAULT_BUILD CL_HPP_ENABLE_EXCEPTIONS)
    target_compile_definitions(cl-tool-unit-tests PRIVATE __CL_ENABLE_EXCEPTIONS CL_VERSION_1_2)
    target_include_directories(cl-tool-unit-tests PRIVATE "${cl_tool_SOURCE_DIR}" ${OPENCL_INCLUDE_DIRS} ${OPENCL2_HPP_INCLUDE_DIRS})
    target_compile_options(cl-tool-unit-tests PRIVATE -Wno-ignored-attributes)
    target_link_libraries(cl-tool-unit-tests ${OPENCL_LIBRARIES} gtest_main Threads::Threads)

    add_test(NAME unit-test COMMAND cl-tool-unit-tests)
endif()

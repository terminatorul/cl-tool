cmake_minimum_required(VERSION 3.2)
project(cl-tool CXX)

include(UseOpenCL.cmake)
include(UseGoogleTest.cmake)

find_package(Threads)

set(CL_TOOL_SOURCES parse-cmd-line.hh parse-cmd-line.cc cl-matrix-mult.hh cl-matrix-mult.cc cl-platform-info.hh cl-platform-info.cc cl-tool.cc)
set(CL_TOOL_TARGET_SOURCES cl-matrix-rand.cl)

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

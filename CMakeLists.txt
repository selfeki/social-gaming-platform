cmake_minimum_required(VERSION 3.12)
project(SocialGaming)

set(MY_PROJECT_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}")
set(MY_PROJECT_MODULE_DIR "${MY_PROJECT_ROOT_DIR}/module")
set(MY_PROJECT_CONFIG_CXX "${MY_PROJECT_ROOT_DIR}/scripts/cmake/cxx.cmake")
set(MY_PROJECT_CONFIG_COMMON "${MY_PROJECT_ROOT_DIR}/scripts/cmake/common.cmake")

set(PACKAGE_NAME WebSocketNetworking)
set(PACKAGE_VERSION 0.2)
set(PACKAGE_STRING "${PACKAGE_NAME} ${PACKAGE_VERSION}")
set(PACKAGE_BUGREPORT "wsumner@sfu.ca")

# Include project modules.
file(GLOB MY_PROJECT_MODULE_LISTS "${MY_PROJECT_MODULE_DIR}/*/CMakeLists.txt")
foreach(module_file "${MY_PROJECT_MODULE_LISTS}")
	get_filename_component(module_dir "${module_file}" DIRECTORY)
	get_filename_component(module_name "${module_dir}" NAME)
	message("-- Found project module: ${module_name}")
	add_subdirectory("${module_dir}")
endforeach()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -fno-rtti")
set(CMAKE_CXX_FLAGS_DEBUG 
    "-O0 -g -fsanitize=address,undefined -fno-omit-frame-pointer"
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib")

add_subdirectory(lib)
add_subdirectory(tools)


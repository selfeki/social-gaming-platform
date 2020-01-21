project(Untitled)

set(MY_PROJECT_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}")
set(MY_PROJECT_MODULE_DIR "${MY_PROJECT_ROOT_DIR}/module")
set(MY_PROJECT_CONFIG_CXX "${MY_PROJECT_ROOT_DIR}/scripts/cmake/cxx.cmake")
set(MY_PROJECT_CONFIG_COMMON "${MY_PROJECT_ROOT_DIR}/scripts/cmake/common.cmake")

# Include project modules.
file(GLOB MY_PROJECT_MODULE_LISTS "${MY_PROJECT_MODULE_DIR}/*/CMakeLists.txt")
foreach(module_file "${MY_PROJECT_MODULE_LISTS}")
	get_filename_component(module_dir "${module_file}" DIRECTORY)
	get_filename_component(module_name "${module_dir}" NAME)
	message("-- Found project module: ${module_name}")
	add_subdirectory("${module_dir}")
endforeach()


cmake_minimum_required(VERSION 3.10)
project(SocialGaming)

# Configure environment-specific build variables.
# These can be used to tweak the build process for the current environment.
if(EXISTS "${CMAKE_CURRENT_LIST_DIR}/.buildenv")
	include("${CMAKE_CURRENT_LIST_DIR}/.buildenv")
endif()

# Configure Arepa build variables.
set(AREPA_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}")
set(AREPA_MODULE_DIR "${AREPA_ROOT_DIR}/module")
set(AREPA_EXTERNAL_MODULE_DIR "${AREPA_ROOT_DIR}/extern")
set(AREPA_CONFIG_CXX "${AREPA_ROOT_DIR}/scripts/cmake/cxx.cmake")
set(AREPA_CONFIG_COMMON "${AREPA_ROOT_DIR}/scripts/cmake/common.cmake")

# Configure project variables.
set(PACKAGE_NAME SocialGaming)
set(PACKAGE_VERSION 0.1)
set(PACKAGE_STRING "${PACKAGE_NAME} ${PACKAGE_VERSION}")
set(PACKAGE_BUGREPORT "https://csil-git1.cs.surrey.sfu.ca/373-20-1-arepa/social-gaming/issues")

# Include cmake scripts.
include("${CMAKE_CURRENT_LIST_DIR}/scripts/cmake/arepa_target_info.cmake")

# Include project modules.
file(GLOB AREPA_MODULE_LISTS "${AREPA_MODULE_DIR}/*/CMakeLists.txt")
foreach(module_file ${AREPA_MODULE_LISTS})
	get_filename_component(module_dir "${module_file}" DIRECTORY)
	get_filename_component(module_name "${module_dir}" NAME)
	if (NOT "${module_name}" IN_LIST SKIP_MODULES)
		message("-- Found project module: ${module_name}")
		add_subdirectory("${module_dir}")
	else()
		message("-- Skipping project module: ${module_name}")
	endif()
endforeach()

# Generate a list of project files for use in other tooling.
# This will be saved as "ArepaProjectFiles.txt"
get_property(targets GLOBAL PROPERTY AREPA_TARGETS)
arepa_get_target_sources(project_sources "${targets}")
arepa_get_target_headers(project_headers "${targets}")
set(project_files "${project_sources}")
list(APPEND project_files ${project_headers})
list(JOIN project_files "\n" project_files_joined)
file(WRITE "${CMAKE_BINARY_DIR}/ArepaProjectFiles.txt" "${project_files_joined}")
message("-- Generated project file list")

# Add format target.
include("${CMAKE_CURRENT_LIST_DIR}/scripts/cmake/incl_target_format.cmake")

# ----------------------------------------------------------------------------------------------------------------------
# Configuration:
# ----------------------------------------------------------------------------------------------------------------------
SET(AREPA_DEFAULT_CXX_STANDARD 17)
SET(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} \
	-pedantic -Wall -Wextra -Wfloat-equal -Wold-style-cast \
	-Wzero-as-null-pointer-constant -Wshadow -Wnon-virtual-dtor -Wint-to-pointer-cast \
	-Wno-unused -Wno-unused-parameter"
)

if(CMAKE_COMPILER_IS_GNUCC)
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-shadow -Wno-subobject-linkage -Wno-zero-as-null-pointer-constant")
endif()

if(NOT AREPA_SKIP_LINTING)
	SET(CMAKE_CXX_CLANG_TIDY clang-tidy)
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")
endif()

# ----------------------------------------------------------------------------------------------------------------------
# Include:
# ----------------------------------------------------------------------------------------------------------------------
include("${CMAKE_CURRENT_LIST_DIR}/common.cmake")
file(GLOB _lib_scripts "${CMAKE_CURRENT_LIST_DIR}/lib_*.cmake")
foreach(_lib_script ${_lib_scripts})
	include("${_lib_script}")
endforeach()

# ----------------------------------------------------------------------------------------------------------------------
# Functions:
# ----------------------------------------------------------------------------------------------------------------------

# Function:    arepa_target_cxx
# Description:
#
#     Configures a pre-existing target to use the Arepa C++ module setup and layout.
#
#     Layout:
#      * `include` - Exported header files.
#      * `src`     - Private sources.
#
function(arepa_target_cxx module_name)
	# Set the include directories.
	target_include_directories("${module_name}"
		PRIVATE
			$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
			${CMAKE_CURRENT_SOURCE_DIR}/src
		PUBLIC
			$<INSTALL_INTERFACE:include>
	)

	# Set the exported include directories.
	get_target_property(header_prefix "${module_name}" AREPA_HEADER_PREFIX)
	if(header_prefix)
		arepa_export_headers("${module_name}" "${CMAKE_CURRENT_SOURCE_DIR}/include")
	endif()

	# Set the source directories.
	file(GLOB "${module_name}_SRC"
		"${CMAKE_CURRENT_SOURCE_DIR}/src/*.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc"
		"${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
		"${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
		"${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp"
	)
	target_sources("${module_name}" PRIVATE ${${module_name}_SRC})

	# Configure the C++ properties.
	set_target_properties("${module_name}"
		PROPERTIES
			LINKER_LANGUAGE CXX
			CXX_STANDARD "${AREPA_DEFAULT_CXX_STANDARD}"
	)
endfunction(arepa_target_cxx)

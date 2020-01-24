# ----------------------------------------------------------------------------------------------------------------------
# Include:
# ----------------------------------------------------------------------------------------------------------------------
include("${CMAKE_CURRENT_LIST_DIR}/fn_export_headers.cmake")

# ----------------------------------------------------------------------------------------------------------------------
# Functions:
# ----------------------------------------------------------------------------------------------------------------------

# Function:    arepa_create_module
# Description:
#
#     Creates a module target.
#     This will generate a static library that can be re-used in other modules or executables.
#
# Arguments:
#
#     * `target_name`   - The target name.
#     * `export_prefix` - The library's export prefix path.
#                         If this is set to "example", headers would be included as "example/Header.hpp".
#
function(arepa_create_module target_name export_prefix)
	message("   --> Creates static library \"${target_name}\" (as \"${export_prefix}\")")

	# Create the build target.
	add_library("${target_name}")

	# Configure the install command.
	install(TARGETS "${target_name}"
		ARCHIVE DESTINATION lib
		PUBLIC_HEADER DESTINATION "include/${export_prefix}"
	)

	# Set variables.
	set_target_properties("${target_name}"
		PROPERTIES
			AREPA_HEADER_PREFIX "${export_prefix}"
	)
endfunction(arepa_create_module)


# Function:    arepa_create_executable
# Description:
#
#     Creates an executable target.
#     This will create an executable file.
#
# Arguments:
#
#     * `target_name`     - The target name.
#     * `executable_name` - The output filename for the executable.
#
function(arepa_create_executable target_name executable_name)
	message("   --> Creates executable \"${executable_name}\"")

	# Create the build target.
	add_executable("${target_name}")
	set_target_properties(${target_name} PROPERTIES OUTPUT_NAME "${executable_name}")

	# Configure the install command.
	install(TARGETS "${target_name}"
		RUNTIME DESTINATION bin
	)
endfunction(arepa_create_executable)

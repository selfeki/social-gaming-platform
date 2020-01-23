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
function(arepa_create_module module_name export_prefix)
	message("   --> Creates static library \"${module_name}\" (as \"${export_prefix}\")")

	# Create the build target.
	add_library("${module_name}")

	# Configure the install command.
	install(TARGETS "${module_name}"
		ARCHIVE DESTINATION lib
		PUBLIC_HEADER DESTINATION "include/${export_prefix}"
	)

	# Set variables.
	set_target_properties("${module_name}"
		PROPERTIES
			AREPA_HEADER_PREFIX "${export_prefix}"
	)
endfunction(arepa_create_module)


# Function:    arepa_create_module
# Description:
#
#     Creates an executable target.
#     This will generate a static library that can be re-used in other modules or executables.
#
function(arepa_create_executable module_name executable_name)
	message("   --> Creates executable \"${executable_name}\"")

	# Create the build target.
	add_executable("${module_name}")
	set_target_properties(${module_name} PROPERTIES OUTPUT_NAME "${executable_name}")

	# Configure the install command.
	install(TARGETS "${module_name}"
		RUNTIME DESTINATION bin
	)
endfunction(arepa_create_executable)

# ----------------------------------------------------------------------------------------------------------------------
# Functions:
# ----------------------------------------------------------------------------------------------------------------------

# Function:    arepa_create_module
# Description:
#
#     Creates a module target.
#     This will generate a static library that can be re-used in other modules or executables.
#
function(arepa_create_module module_name)
	message("   --> Creates static library \"${module_name}\"")

	# Create the build target.
	add_library("${module_name}")

	# Configure the install command.
	install(TARGETS "${module_name}"
		ARCHIVE DESTINATION lib
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

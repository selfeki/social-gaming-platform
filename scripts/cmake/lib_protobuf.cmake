# ----------------------------------------------------------------------------------------------------------------------
# Functions:
# ----------------------------------------------------------------------------------------------------------------------

# Function:    arepa_use_protobuf
# Description:
#
#     Configures a pre-existing target to compile and export Protobuf sources.
#
# Arguments:
#
#     * `target_name`     - The target name.
#
function(arepa_compile_protobuf target_name)
	find_package(Protobuf REQUIRED)

	# Find all the protobuf files for the target.
	get_target_property(target_dir "${target_name}" SOURCE_DIR)
	get_target_property(output_dir "${target_name}" BINARY_DIR)
	file(GLOB proto_files "${target_dir}/proto/*.proto")
	set(header_output_dir "${output_dir}/generated_headers")

	# Ensure that it rebuilds protobuf files.

	# Generate source code using protobuf.
	protobuf_generate_cpp(generated_sources generated_headers ${proto_files})

	# Add generated sources and headers to project.
	target_sources("${target_name}" PRIVATE ${generated_sources})
	target_include_directories("${target_name}" PRIVATE "${header_output_dir}")
	target_include_directories("${target_name}" PUBLIC "${header_output_dir}")

	# Add (wrapped) generated headers to project.
	foreach(generated_header ${generated_headers})
		file(RELATIVE_PATH generated_header_name "${output_dir}" "${generated_header}")
		get_filename_component(header_name_noext "${generated_header_name}" NAME_WE)
		get_filename_component(header_name_parent "${generated_header_name}" DIRECTORY)

		if (NO_SYMLINKS)
			file(WRITE "${header_output_dir}/${header_name_parent}/${header_name_noext}.hpp" "#include <${generated_header}>")
		else()
			file(TOUCH "${generated_header}")
			set(destination_link "${header_output_dir}/${header_name_parent}${header_name_noext}.hpp")
			get_filename_component(destination_link_parent "${destination_link}" DIRECTORY)
			file(MAKE_DIRECTORY "${destination_link_parent}")
			file(CREATE_LINK "${generated_header}" "${destination_link}" SYMBOLIC)
		endif()
	endforeach()

	# Export!
	arepa_export_headers("${target_name}" "${header_output_dir}")
endfunction(arepa_compile_protobuf)

# Function:    arepa_use_protobuf
# Description:
#
#     Configures a pre-existing target to use Protobuf.
#
# Arguments:
#
#     * `target_name`     - The target name.
#     * `scope`?          - The library scope. Use PUBLIC if headers are re-exported.
#
function(arepa_use_protobuf target_name)
	find_package(Protobuf REQUIRED)

	if("${ARGC}" GREATER 1)
		set(scope "${ARGV1}")
	else()
		set(scope PRIVATE)
	endif()

	target_link_libraries("${target_name}" "${scope}" ${Protobuf_LIBRARIES})
	target_include_directories("${target_name}" SYSTEM "${scope}" ${Protobuf_INCLUDE_DIRS})
endfunction(arepa_use_protobuf)

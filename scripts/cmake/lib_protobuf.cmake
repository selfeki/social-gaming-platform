# ----------------------------------------------------------------------------------------------------------------------
# Functions:
# ----------------------------------------------------------------------------------------------------------------------

# Function:    arepa_use_protobuf
# Description:
#
#     Configures a pre-existing target to use Protobuf.
#
# Arguments:
#
#     * `target_name`     - The target name.
#
function(arepa_use_protobuf target_name)
	find_package(Protobuf REQUIRED)

	# Find all the protobuf files for the target.
	get_target_property(target_dir "${target_name}" SOURCE_DIR)
	get_target_property(output_dir "${target_name}" BINARY_DIR)
	file(GLOB proto_files "${target_dir}/proto/*.proto")
	set(header_output_dir "${output_dir}/generated_headers")

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

		file(WRITE "${header_output_dir}/${header_name_parent}/${header_name_noext}.hpp" "#include \"${generated_header}\"")
	endforeach()

	# Export!
	arepa_export_headers("${target_name}" "${header_output_dir}")
endfunction(arepa_use_protobuf)

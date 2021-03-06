# ----------------------------------------------------------------------------------------------------------------------
# Functions:
# ----------------------------------------------------------------------------------------------------------------------

function(arepa_export_headers target_name header_dirs)
	# Get the target's module directory.
	get_target_property(target_dir "${target_name}" SOURCE_DIR)

	# Get the target's generated project directory.
	get_target_property(output_dir "${target_name}" BINARY_DIR)

	# Determine and create the directory which will contain the exported headers.
	# This is going to be inside the module's generated CMakeFiles directory as "export_headers".
	get_target_property(export_header_prefix "${target_name}" AREPA_HEADER_PREFIX)
	set(export_header_dir "${output_dir}${CMAKE_FILES_DIRECTORY}/export_headers")
	set(export_header_dir_with_prefix "${export_header_dir}/${export_header_prefix}")
	file(MAKE_DIRECTORY "${export_header_dir_with_prefix}")
	target_include_directories("${target_name}" PUBLIC "$<BUILD_INTERFACE:${export_header_dir}>")

	# Symlink each header file.
	foreach(header_dir ${header_dirs})
		file(GLOB_RECURSE header_files "${header_dir}/*")
		foreach(header_file ${header_files})
			file(RELATIVE_PATH header_file_relative "${header_dir}" "${header_file}")
			get_filename_component(header_file_relative_dirname "${header_file_relative}" DIRECTORY)

			# Create the symlink.
			file(MAKE_DIRECTORY "${export_header_dir_with_prefix}/${header_file_relative_dirname}")
			if (NO_SYMLINKS)
				configure_file("${header_file}" "${export_header_dir_with_prefix}/${header_file_relative}")
				file(COPY "${header_file}" DESTINATION "${export_header_dir_with_prefix}")
			else()
				file(CREATE_LINK "${header_file}" "${export_header_dir_with_prefix}/${header_file_relative}" SYMBOLIC)
			endif()
		endforeach()
	endforeach()
endfunction(arepa_export_headers)

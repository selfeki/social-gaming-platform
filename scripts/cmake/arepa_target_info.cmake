# ----------------------------------------------------------------------------------------------------------------------
# Functions:
# ----------------------------------------------------------------------------------------------------------------------

# Function:    arepa_get_target_sources
# Description:
#
#     Extracts a list of source files specified in a target.
#
# Arguments:
#
#     * `out_var`       - The output variable.
#     * `target_name`   - The target name.
#
function(arepa_get_target_sources out_var target_name)
	set(files "")
	foreach(target ${target_name})
		get_target_property(target_sources "${target}" SOURCES)
		list(APPEND files ${target_sources})
	endforeach()
	set("${out_var}" "${files}" PARENT_SCOPE)
endfunction(arepa_get_target_sources)

# Function:    arepa_get_target_headers
# Description:
#
#     Extracts a list of header files (roughly) specified in a target.
#
# Arguments:
#
#     * `out_var`       - The output variable.
#     * `target_name`   - The target name.
#
function(arepa_get_target_headers out_var target_name)
	set(files "")
	foreach(target ${target_name})
		get_target_property(target_dir "${target}" SOURCE_DIR)
		get_target_property(target_include_dirs "${target}" INCLUDE_DIRECTORIES)
		foreach (include_dir ${target_include_dirs})
			# If it's a generator, extract the absolute path.
			if ("${include_dir}" MATCHES "^\\$<BUILD_INTERFACE:(.*)>$")
				set(include_dir "${CMAKE_MATCH_1}")
			elseif ("${include_dir}" MATCHES "^\\$")
				# We ignore any other generators.
				continue()
			endif()

			# If it's outside the module directory, skip it.
			file(RELATIVE_PATH include_dir_relative "${target_dir}" "${include_dir}")
			if ("${include_dir_relative}" MATCHES "^\\.")
				continue()
			endif()

			# Recursively glob all the header files.
			file(GLOB_RECURSE target_headers
					"${include_dir}/*.hpp"
					"${include_dir}/*.h"
			)

			list(APPEND files ${target_headers})
		endforeach()
	endforeach()
	set("${out_var}" "${files}" PARENT_SCOPE)
endfunction(arepa_get_target_headers)

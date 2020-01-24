# A script, that when included, creates a `format` target which calls `clang-format` on all the
# header and sources files in the project.
#
# This depends on a `ArepaProjectFiles.txt` file being generated in the output directory.

find_program(
	CLANG_FORMAT_EXE
	NAMES "clang-format"
	DOC "Path to clang-format executable"
)

if (NOT CLANG_FORMAT_EXE)
	message(AUTHOR_WARNING "Could not find clang-format executable. The 'format' target will not be generated.")
else()
	if (NOT TARGET format)
		add_custom_target(format)
	endif()

	add_custom_command(TARGET format POST_BUILD
		COMMAND tr '\\n' '\\0' "<${CMAKE_BINARY_DIR}/ArepaProjectFiles.txt" "|" xargs -0 -n 50 "${CLANG_FORMAT_EXE}" --verbose -i
	)
endif()

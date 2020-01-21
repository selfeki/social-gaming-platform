#!/usr/bin/env bash
# ----------------------------------------------------------------------------------------------------------------------
# <project name>, copyright 2020.
# A script to determine system information for a CMake build.
# ----------------------------------------------------------------------------------------------------------------------
set -e -o pipefail
TEMP="$(mktemp -d -t cmakeXXXX)"
cat > "${TEMP}/CMakeLists.txt" <<'EOF'
	project("build info query")

	# Determine the system ABI (e.g. gnu, armhf, etc.):
	if(DEFINED CMAKE_LIBRARY_ARCHITECTURE)
		string(REGEX MATCH "-[a-zA-Z0-9]+$" X_SYSTEM_ABI "${CMAKE_LIBRARY_ARCHITECTURE}")
		string(REGEX MATCH "[^-]+$" X_SYSTEM_ABI "${X_SYSTEM_ABI}")
	else()
	endif()

	# Determine the system variant (Ubuntu, Arch, etc.):
	find_program(LSB_RELEASE_EXEC lsb_release)
	if(DEFINED X_SYSTEM_VARIANT)
	elseif(LSB_RELEASE_EXEC)
		execute_process(COMMAND ${LSB_RELEASE_EXEC} -is
			OUTPUT_VARIABLE X_SYSTEM_VARIANT
			OUTPUT_STRIP_TRAILING_WHITESPACE
		)
		string(TOLOWER X_SYSTEM_VARIANT "${X_SYSTEM_VARIANT}")
	elseif(DEFINED CMAKE_EFFECTIVE_SYSTEM_NAME AND NOT CMAKE_SYSTEM_NAME STREQUAL CMAKE_EFFECTIVE_SYSTEM_NAME)
		set(X_SYSTEM_VARIANT "${CMAKE_EFFECTIVE_SYSTEM_NAME}")
	else()
		set(X_SYSTEM_VARIANT "Unknown")
	endif()

	# Determine the system string (e.g. x86_64-unknown-linux-gnu):
	set(X_SYSTEM_STRING "${CMAKE_SYSTEM_PROCESSOR}-${X_SYSTEM_VARIANT}-${CMAKE_SYSTEM_NAME}")
	if(X_SYSTEM_ABI)
		set(X_SYSTEM_STRING "${X_SYSTEM_STRING}-${X_SYSTEM_ABI}")
	endif()
	string(TOLOWER "${X_SYSTEM_STRING}" X_SYSTEM_STRING)

	
	message(">SYSTEM_PROCESSOR:${CMAKE_SYSTEM_PROCESSOR}")
	message(">SYSTEM_NAME:${CMAKE_SYSTEM_NAME}")
	message(">SYSTEM_VARIANT:${X_SYSTEM_VARIANT}")
	message(">SYSTEM_ABI:${X_SYSTEM_ABI}")
	message(">SYSTEM_STRING:${X_SYSTEM_STRING}")
	message(">BUILD_COMMAND:${CMAKE_MAKE_COMMAND}")
EOF

# Run the cmake command.
({
	cd "$TEMP"
	while read -r line; do
		if [[ "${line:0:1}" = ">" ]]; then
			printf "%s=%q\n" "$(cut -d':' -f1 <<< "${line:1}")" "$(cut -d':' -f2- <<< "$line")"
			continue
		fi

		echo "$line"
	done < <(cmake . -Wno-dev "$@" 2>&1 | grep -v "^--")
})

# Clean up.
rm -rf "$TEMP"


# ----------------------------------------------------------------------------------------------------------------------
# Configuration:
# ----------------------------------------------------------------------------------------------------------------------
SET(AREPA_BOOST_VERSION 1.66)

# ----------------------------------------------------------------------------------------------------------------------
# Functions:
# ----------------------------------------------------------------------------------------------------------------------

# Function:    arepa_use_boost
# Description:
#
#     Configures a pre-existing target to use the Boost C++ library.
#
# Arguments:
#
#     * `target_name`     - The target name.
#     * `scope`?          - The library scope. Use PUBLIC if headers are re-exported.
#
function(arepa_use_boost target_name)
	find_package(Boost "${AREPA_BOOST_VERSION}" COMPONENTS system REQUIRED)

	if("${ARGC}" GREATER 1)
		set(scope "${ARGV1}")
	else()
		set(scope PRIVATE)
	endif()

	target_include_directories("${target_name}"
		SYSTEM "${scope}"
			${Boost_INCLUDE_DIR}
	)

	target_link_libraries("${target_name}"
		"${scope}"
			${Boost_LIBRARIES}
	)

	# Fix issue with Boost and C++17.
	target_compile_definitions("${target_name}"
		"${scope}"
			_LIBCPP_ENABLE_CXX17_REMOVED_AUTO_PTR
	)
endfunction(arepa_use_boost)


# Function:    arepa_use_boost_beast
# Description:
#
#     Configures a pre-existing target to use the Boost.Beast HTTP/WebSocket library.
#
# Arguments:
#
#     * `target_name`     - The target name.
#     * `scope`?          - The library scope. Use PUBLIC if headers are re-exported.
#
function(arepa_use_boost_beast target_name)
	if("${ARGC}" GREATER 1)
		set(scope "${ARGV1}")
	else()
		set(scope PRIVATE)
	endif()

	target_include_directories("${target_name}"
		SYSTEM "${scope}"
			"${AREPA_EXTERNAL_MODULE_DIR}/boost_beast/include"
	)
endfunction(arepa_use_boost_beast)

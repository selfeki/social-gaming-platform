# ----------------------------------------------------------------------------------------------------------------------
# Configuration:
# ----------------------------------------------------------------------------------------------------------------------
SET(AREPA_BOOST_VERSION 1.70)

# ----------------------------------------------------------------------------------------------------------------------
# Functions:
# ----------------------------------------------------------------------------------------------------------------------

# Function:    arepa_use_boost
# Description:
#
#     Configures a pre-existing target to use the Boost C++ library.
#
function(arepa_use_boost module_name)
	find_package(Boost "${AREPA_BOOST_VERSION}" COMPONENTS system REQUIRED)

	target_include_directories("${module_name}"
		PRIVATE
			${Boost_INCLUDE_DIR}
	)

	target_link_libraries("${module_name}"
		PRIVATE
			${Boost_LIBRARIES}
	)

	# Fix issue with Boost and C++17.
	target_compile_definitions("${module_name}"
		PRIVATE
			_LIBCPP_ENABLE_CXX17_REMOVED_AUTO_PTR
	)
endfunction(arepa_use_boost)


# Function:    arepa_use_boost_beast
# Description:
#
#     Configures a pre-existing target to use the Boost.Beast HTTP/WebSocket library.
#
function(arepa_use_boost_beast module_name)
	target_include_directories("${module_name}"
		PRIVATE
			"${AREPA_EXTERNAL_MODULE_DIR}/boost_beast/include"
	)
endfunction(arepa_use_boost_beast)

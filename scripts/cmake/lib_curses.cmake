# ----------------------------------------------------------------------------------------------------------------------
# Functions:
# ----------------------------------------------------------------------------------------------------------------------

# Function:    arepa_use_curses
# Description:
#
#     Configures a pre-existing target to use the system curses library.
#
function(arepa_use_curses module_name)
	find_package(Curses REQUIRED)

	target_link_libraries("${module_name}"
		${CURSES_LIBRARIES}
	)
endfunction(arepa_use_curses)

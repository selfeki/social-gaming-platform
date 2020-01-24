# ----------------------------------------------------------------------------------------------------------------------
# Functions:
# ----------------------------------------------------------------------------------------------------------------------

# Function:    arepa_use_threads
# Description:
#
#     Configures a pre-existing target to use the native multithreading library.
#
function(arepa_use_threads module_name)
	find_package(Threads REQUIRED)

	target_link_libraries("${module_name}"
		${CMAKE_THREAD_LIBS_INIT}
	)
endfunction(arepa_use_threads)

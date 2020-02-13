include("${AREPA_CONFIG_CXX}")

# Create module.
arepa_create_executable(server_daemon "chatserver") # TODO: Rename the executable.
arepa_target_cxx(server_daemon)

arepa_use_threads(server_daemon)
target_link_libraries(server_daemon networking)
target_link_libraries(server_daemon command)
target_link_libraries(server_daemon server)
target_link_libraries(server_daemon jsonconfig)
target_link_libraries(server_daemon game_manager)


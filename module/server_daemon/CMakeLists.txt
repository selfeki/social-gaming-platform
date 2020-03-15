include("${AREPA_CONFIG_CXX}")

# Create module.
arepa_create_executable(server_daemon "chatserver")
arepa_target_cxx(server_daemon)

arepa_use_threads(server_daemon)
target_link_libraries(server_daemon log)
target_link_libraries(server_daemon log_adapter_console)
target_link_libraries(server_daemon networking)
target_link_libraries(server_daemon command)
target_link_libraries(server_daemon server)
target_link_libraries(server_daemon game_manager)
target_link_libraries(server_daemon game)
target_link_libraries(server_daemon server_config)
target_link_libraries(server_daemon serializer)

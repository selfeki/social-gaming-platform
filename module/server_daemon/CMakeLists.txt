include("${AREPA_CONFIG_CXX}")

# Create module.
find_package(nlohmann_json 3.2.0 REQUIRED)

arepa_create_executable(server_daemon "chatserver" "DSLInterpreter") # TODO: Rename the executable.
arepa_target_cxx(server_daemon)

arepa_use_threads(server_daemon)
target_link_libraries(server_daemon PRIVATE networking)
target_link_libraries(server_daemon PRIVATE nlohmann_json::nlohmann_json)

include("${AREPA_CONFIG_CXX}")

# Create module.
arepa_create_module(server "arepa/server")
arepa_target_cxx(server)

target_link_libraries(server PUBLIC networking)
target_link_libraries(server PUBLIC networking_websocket)
target_link_libraries(server PUBLIC protocol)

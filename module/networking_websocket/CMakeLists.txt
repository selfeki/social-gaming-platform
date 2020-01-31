include("${AREPA_CONFIG_CXX}")

# Create module.
arepa_create_module(networking_websocket "arepa/networking/websocket")
arepa_target_cxx(networking_websocket)

target_link_libraries(networking_websocket PUBLIC common_communication)
target_link_libraries(networking_websocket PUBLIC networking)

arepa_use_boost(networking_websocket)
arepa_use_boost_beast(networking_websocket)

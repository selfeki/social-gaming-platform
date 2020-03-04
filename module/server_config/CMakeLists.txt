include("${AREPA_CONFIG_CXX}")  

# Create module.
arepa_create_module(server_config "arepa/server_config") 
arepa_target_cxx(server_config)	

target_link_libraries(server_config INTERFACE game_spec)
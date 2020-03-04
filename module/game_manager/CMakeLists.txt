include("${AREPA_CONFIG_CXX}")       

arepa_create_module(game_manager ".") 
arepa_target_cxx(game_manager)	

target_link_libraries(game_manager PUBLIC jsonconfig)
target_link_libraries(game_manager PUBLIC command)
target_link_libraries(game_manager PUBLIC server)
target_link_libraries(game_manager PUBLIC game_spec)
arepa_use_boost(game_manager)

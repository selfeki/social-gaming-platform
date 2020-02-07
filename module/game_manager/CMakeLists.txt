include("${AREPA_CONFIG_CXX}")       

arepa_create_module(game_manager ".") 
arepa_target_cxx(game_manager)	

target_link_libraries(game_manager networking)
target_link_libraries(game_manager cereal)
target_link_libraries(game_manager command)
arepa_use_boost(game_manager)
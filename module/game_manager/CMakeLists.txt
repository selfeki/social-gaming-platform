include("${AREPA_CONFIG_CXX}")       

arepa_create_module(game_manager ".") 
arepa_target_cxx(game_manager)	

target_link_libraries(game_manager INTERFACE cereal)
target_link_libraries(game_manager PRIVATE command)
arepa_use_boost(game_manager)
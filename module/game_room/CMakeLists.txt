include("${AREPA_CONFIG_CXX}")       

arepa_create_module(game_room "arepa/game/room") 
arepa_target_cxx(game_room)	

target_link_libraries(game_room PUBLIC command)
target_link_libraries(game_room PUBLIC networking)
target_link_libraries(game_room PUBLIC log)
arepa_use_boost(game_room)

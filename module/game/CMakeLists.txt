include("${AREPA_CONFIG_CXX}")       

arepa_create_module(game "arepa/game")
arepa_target_cxx(game)	

target_link_libraries(game PUBLIC command)
target_link_libraries(game PUBLIC chat)
target_link_libraries(game PUBLIC common)
target_link_libraries(game PUBLIC log)

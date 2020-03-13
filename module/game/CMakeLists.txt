include("${AREPA_CONFIG_CXX}")       

arepa_create_module(game "arepa/game")
arepa_target_cxx(game)	

target_link_libraries(game PUBLIC command)
target_link_libraries(game PUBLIC common)
target_link_libraries(game PUBLIC networking)
target_link_libraries(game PUBLIC log)
target_link_libraries(game PUBLIC protocol)

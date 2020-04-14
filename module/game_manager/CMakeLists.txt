include("${AREPA_CONFIG_CXX}")       

arepa_create_module(game_manager ".") 
arepa_target_cxx(game_manager)	

arepa_use_boost(game_manager)

target_link_libraries(game_manager PUBLIC server_config)
target_link_libraries(game_manager PUBLIC common)
target_link_libraries(game_manager PUBLIC command)
target_link_libraries(game_manager PUBLIC server)
target_link_libraries(game_manager PUBLIC game_spec)
target_link_libraries(game_manager PUBLIC game)
target_link_libraries(game_manager PUBLIC chat)


# Create temporary driver for testing
arepa_create_executable(game_manager_tests "game_manager_tests")
arepa_target_cxx(game_manager_tests)
target_link_libraries(game_manager_tests game_manager)

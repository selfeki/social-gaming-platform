include("${AREPA_CONFIG_CXX}")       

arepa_create_module(game_manager ".") 
arepa_target_cxx(game_manager)	

target_link_libraries(game_manager PUBLIC server_config)
target_link_libraries(game_manager PUBLIC command)
target_link_libraries(game_manager PUBLIC server)
target_link_libraries(game_manager PUBLIC game_spec)
arepa_use_boost(game_manager)

arepa_create_executable(interpreter_test "interpreter_test")
arepa_target_cxx(interpreter_test)
target_link_libraries(interpreter_test game_manager)
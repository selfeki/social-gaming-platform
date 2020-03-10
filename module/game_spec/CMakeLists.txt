include("${AREPA_CONFIG_CXX}")       

# Create module.
arepa_create_module(game_spec "arepa/game_spec") 
arepa_target_cxx(game_spec) 

arepa_use_boost(game_spec PUBLIC)

# Create driver for testing Expression visitation
arepa_create_executable(game_spec_test "test")
arepa_target_cxx(game_spec_test)
target_link_libraries(game_spec_test game_spec)
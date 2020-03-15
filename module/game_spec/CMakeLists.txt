include("${AREPA_CONFIG_CXX}")

# Create module.
arepa_create_module(game_spec "arepa/game_spec")
arepa_target_cxx(game_spec)

arepa_use_boost(game_spec PUBLIC)

# Create temporary driver for testing
arepa_create_executable(game_spec_tests "game_spec_tests")
arepa_target_cxx(game_spec_tests)
target_link_libraries(game_spec_tests game_spec)

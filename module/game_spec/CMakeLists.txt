include("${AREPA_CONFIG_CXX}")       

# Create module.
arepa_create_module(game_spec "arepa/game_spec") 
arepa_target_cxx(game_spec)	

arepa_use_boost(game_spec)
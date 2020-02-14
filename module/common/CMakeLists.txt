include("${AREPA_CONFIG_CXX}")

# Create module.
arepa_create_module(common "arepa")
arepa_target_cxx(common)

arepa_use_boost(common)

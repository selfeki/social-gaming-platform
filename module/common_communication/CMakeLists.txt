include("${AREPA_CONFIG_CXX}")

# Create module.
arepa_create_module(common_communication "arepa/communication")
arepa_target_cxx(common_communication)

arepa_use_boost(common_communication)

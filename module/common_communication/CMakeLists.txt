include("${AREPA_CONFIG_CXX}")

# Create module.
arepa_create_module(common_communication "arepa/communication")
arepa_target_cxx(common_communication)

target_link_libraries(common_communication PUBLIC common_collections)

arepa_use_boost(common_communication)

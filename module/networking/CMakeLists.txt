include("${AREPA_CONFIG_CXX}")

# Create module.
arepa_create_module(networking "arepa/networking")
arepa_target_cxx(networking)

target_link_libraries(networking PUBLIC common_communication)

arepa_use_boost(networking)
arepa_use_boost_beast(networking)

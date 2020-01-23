# Load common configurations.
include("${AREPA_CONFIG_COMMON}")
include("${AREPA_CONFIG_CXX}")

# Create module.
arepa_create_module(networking)
arepa_target_cxx(networking)

arepa_use_boost(networking)
arepa_use_boost_beast(networking)

include("${AREPA_CONFIG_CXX}")

# Create module.
arepa_create_module(common_collections "arepa/collections")
arepa_target_cxx(common_collections)

arepa_use_boost(common_collections)

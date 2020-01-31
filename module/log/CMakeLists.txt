include("${AREPA_CONFIG_CXX}")

# Create module.
arepa_create_module(log "arepa/log")
arepa_target_cxx(log)

target_link_libraries(log PUBLIC common_collections)
arepa_use_boost(log)

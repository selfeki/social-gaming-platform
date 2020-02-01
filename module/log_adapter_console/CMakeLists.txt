include("${AREPA_CONFIG_CXX}")

# Create module.
arepa_create_module(log_adapter_console "arepa/log/console")
arepa_target_cxx(log_adapter_console)

target_link_libraries(log_adapter_console PUBLIC log)
arepa_use_boost(log_adapter_console)

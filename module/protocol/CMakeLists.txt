include("${AREPA_CONFIG_CXX}")

# Create module.
arepa_create_module(protocol "arepa/protocol")
arepa_target_cxx(protocol)

target_link_libraries(protocol PUBLIC networking)

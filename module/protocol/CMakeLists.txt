include("${AREPA_CONFIG_CXX}")

# Create module.
arepa_create_module(protocol "arepa/protocol")
arepa_target_cxx(protocol)

arepa_compile_protobuf(protocol PUBLIC)

target_link_libraries(protocol PUBLIC networking)

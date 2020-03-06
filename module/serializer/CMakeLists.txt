include("${AREPA_CONFIG_CXX}")  

# Create module.
arepa_create_module(serializer "arepa/serializer")
arepa_target_cxx(serializer)

find_package(nlohmann_json 3.2.0 REQUIRED)

target_link_libraries(serializer PUBLIC nlohmann_json::nlohmann_json)
target_link_libraries(serializer PUBLIC server_config)
target_link_libraries(serializer PUBLIC game_spec)

include("${AREPA_CONFIG_CXX}")

# Create module.
arepa_create_executable(interpreter_prototyping "interpreter")
arepa_target_cxx(interpreter_prototyping) 

target_link_libraries(interpreter_prototyping game_spec)

include("${AREPA_CONFIG_CXX}")       

arepa_create_module(command "arepa/command")
arepa_target_cxx(command)	

target_link_libraries(command common)
target_link_libraries(command networking)

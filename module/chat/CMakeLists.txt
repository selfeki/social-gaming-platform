include("${AREPA_CONFIG_CXX}")       

arepa_create_module(chat "arepa/chat")
arepa_target_cxx(chat)

target_link_libraries(chat PUBLIC command)
target_link_libraries(chat PUBLIC common)
target_link_libraries(chat PUBLIC log)
target_link_libraries(chat PUBLIC protocol)
target_link_libraries(chat PUBLIC game)

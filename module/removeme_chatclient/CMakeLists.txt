include("${AREPA_CONFIG_CXX}")

arepa_create_executable(removeme_chatclient "chatclient")
arepa_target_cxx(removeme_chatclient)

arepa_use_curses(removeme_chatclient)
arepa_use_threads(removeme_chatclient)

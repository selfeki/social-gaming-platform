include("${AREPA_CONFIG_CXX}")

add_subdirectory(lib)

add_executable(runAllTests
  # Add all new test files to the list here.
  # Place each file name on a separate line.
  testSerializer.cpp # This file is a placeholder and tests nothing.
)

set_target_properties(runAllTests
  PROPERTIES
    LINKER_LANGUAGE CXX
    CXX_STANDARD "${AREPA_DEFAULT_CXX_STANDARD}"
)

target_link_libraries(runAllTests
  gmock
  gtest
  gtest_main

  # arepa modules
  game_spec
  game_manager
  serializer
  server_config
)

add_test(NAME AllTests COMMAND runAllTests)


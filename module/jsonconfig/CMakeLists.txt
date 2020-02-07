add_library(jsonconfig
  jsonconfig.cpp
)

target_include_directories(jsonconfig
  PUBLIC
    $<INSTALL_INTERFACE:include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

set_target_properties(jsonconfig
                      PROPERTIES
                      LINKER_LANGUAGE CXX
                      CXX_STANDARD 17
)

install(TARGETS jsonconfig
  ARCHIVE DESTINATION lib
)


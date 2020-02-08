add_library(jsonconfig
  jsonconfig.cpp
)

find_package(nlohmann_json 3.2.0 REQUIRED)

target_include_directories(jsonconfig
  PUBLIC
    $<INSTALL_INTERFACE:include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(jsonconfig PUBLIC nlohmann_json::nlohmann_json)

set_target_properties(jsonconfig
                      PROPERTIES
                      LINKER_LANGUAGE CXX
                      CXX_STANDARD 17
)

install(TARGETS jsonconfig
  ARCHIVE DESTINATION lib
)


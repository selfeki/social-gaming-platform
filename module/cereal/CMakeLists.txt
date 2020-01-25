SET (BOOST_ROOT "/usr/local/boost-1.69.0")

add_library(cereal cereal.cpp)

find_package(Boost 1.66 COMPONENTS system REQUIRED)
find_package(nlohmann_json 3.2.0 REQUIRED)

target_include_directories(cereal
  PUBLIC
    $<INSTALL_INTERFACE:include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(cereal PRIVATE nlohmann_json::nlohmann_json)
target_link_libraries(cereal PRIVATE networking)
target_link_libraries(cereal
  PRIVATE
    ${Boost_LIBRARIES}
)

set_target_properties(cereal
                      PROPERTIES
                      LINKER_LANGUAGE CXX
                      CXX_STANDARD 17
)

install(TARGETS cereal
  ARCHIVE DESTINATION lib
)

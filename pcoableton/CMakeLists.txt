add_library(pcoableton STATIC include/pcoableton/pcoapi.h include/pcoableton/pcoapi.cpp)

target_link_libraries(pcoableton PUBLIC cpr::cpr)
target_link_libraries(pcoableton PUBLIC nlohmann_json::nlohmann_json)

target_include_directories(pcoableton PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
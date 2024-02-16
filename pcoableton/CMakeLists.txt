add_library(pcoableton STATIC include/pcoableton/pcoapi.h include/pcoableton/pcoapi.cpp)
target_include_directories(pcoableton PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(pcoableton PRIVATE cpr::cpr)
target_link_libraries(pcoableton PRIVATE json)
target_link_libraries(pcoableton PRIVATE glfw)
target_link_libraries(pcoableton PRIVATE glew)
target_link_libraries(pcoableton PRIVATE imgui)
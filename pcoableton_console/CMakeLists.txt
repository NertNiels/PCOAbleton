add_executable(pcoableton_console main.cpp main.h gui.h gui.cpp)
target_link_libraries(pcoableton_console PRIVATE pcoableton)

target_link_libraries(pcoableton PUBLIC glfw)
target_link_libraries(pcoableton PUBLIC libglew_static)
target_link_libraries(pcoableton PUBLIC imgui)
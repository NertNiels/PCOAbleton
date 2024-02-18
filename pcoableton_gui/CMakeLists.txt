add_executable(pcoableton_gui gui.h gui.cpp)
target_link_libraries(pcoableton_gui PRIVATE pcoableton)

target_link_libraries(pcoableton_gui PUBLIC glfw)
target_link_libraries(pcoableton_gui PUBLIC libglew_static)
target_link_libraries(pcoableton_gui PUBLIC imgui)
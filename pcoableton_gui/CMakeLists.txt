add_executable(pcoableton_gui main.cpp app.cpp app.h)
target_link_libraries(pcoableton_gui PUBLIC pcoableton)

target_link_libraries(pcoableton_gui PUBLIC glfw)
target_link_libraries(pcoableton_gui PUBLIC libglew_static)
target_link_libraries(pcoableton_gui PUBLIC imgui)
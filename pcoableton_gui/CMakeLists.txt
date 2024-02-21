add_executable(pcoableton_gui main.cpp app.cpp app.h audio_handler.h audio_handler.cpp)
target_link_libraries(pcoableton_gui PUBLIC pcoableton)

target_link_libraries(pcoableton_gui PUBLIC glfw)
target_link_libraries(pcoableton_gui PUBLIC libglew_static)
target_link_libraries(pcoableton_gui PUBLIC imgui)
target_link_libraries(pcoableton_gui PUBLIC asio)
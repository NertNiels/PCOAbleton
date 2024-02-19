add_library(pcoableton STATIC include/pcoableton/pcoapi.h include/pcoableton/pcoapi.cpp include/pcoableton/metronome.cpp include/pcoableton/metronome.h include/pcoableton/ableton/AudioEngine.cpp include/pcoableton/ableton/AudioEngine.hpp include/pcoableton/ableton/AudioPlatform_Asio.cpp include/pcoableton/ableton/AudioPlatform_Asio.hpp)

target_link_libraries(pcoableton PUBLIC cpr::cpr)
target_link_libraries(pcoableton PUBLIC nlohmann_json::nlohmann_json)
target_link_libraries(pcoableton PUBLIC Ableton::Link)

target_include_directories(pcoableton PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
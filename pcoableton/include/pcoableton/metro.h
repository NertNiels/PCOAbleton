#include <iostream>
#include <ableton/Link.hpp>

namespace metronome {

    struct state {
        std::atomic<bool> running;
        ableton::Link link;

        state()
            : running(true)
            , link(120.)
        {
        }
    };

    // Getters
    bool link_enabled();
    int link_num_peers();
    double quantum();
    bool playing();
    double current_tempo();
    double link_beat();
    double phase();

    // Setters
    bool link_enabled(bool enabled);
    double quantum(double quantum);
    bool playing(bool playing);
    double current_tempo(double tempo);


    void start();

}
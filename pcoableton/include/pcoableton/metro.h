#include <iostream>
#include <ableton/Link.hpp>

class metronome {
    
    public:
        metronome() : _link(120.) {
            link_enabled(true);
        }

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

    private:
        std::atomic<bool> _running;
        ableton::Link _link;
        const std::chrono::microseconds now() {
            return _link.clock().micros();
        }

};
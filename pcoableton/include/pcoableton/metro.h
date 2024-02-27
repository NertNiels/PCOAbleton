#include <iostream>
#include <ableton/Link.hpp>

#define _USE_MATH_DEFINES
#include <math.h>



class metronome {
    
    public:
        metronome();
        // Getters
        bool link_enabled();
        int link_num_peers();
        double quantum();
        bool playing();
        double current_tempo();
        double link_beat();
        double beat();
        double beat_modifier();
        double link_beatAtTime(std::chrono::microseconds time);
        double beatAtTime(std::chrono::microseconds time);
        double phase();
        double phaseAtTime(std::chrono::microseconds time, double quantum);
        double phaseAtTime(std::chrono::microseconds time);
        double link_phaseAtTime(std::chrono::microseconds time, double quantum);
        double link_phaseAtTime(std::chrono::microseconds time);

        static metronome* global_metro();
        static void set_metro(metronome* metro);   

        // Setters
        bool link_enabled(bool enabled);
        double quantum(double quantum);
        bool playing(bool playing);
        double current_tempo(double tempo);
        double beat_modifier(double mod);

        const std::chrono::microseconds now() {
            return _link.clock().micros();
        }

    private:
        static metronome* _metro;
        std::atomic<bool> _running;
        ableton::Link _link;

        double _beat_modifier = 1;

};
void callback_audio(const double& timeAtStart, const double& sampleAtStart, const size_t& numSamples, std::vector<double>* buffer, std::chrono::microseconds& latency);
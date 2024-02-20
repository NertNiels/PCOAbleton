#include "metro.h"

metronome::state _state;
double _quantum = 4.;

const std::chrono::microseconds now() {
    return _state.link.clock().micros();
}

bool metronome::link_enabled() {
    return _state.link.isEnabled();
}

bool metronome::link_enabled(bool enabled) {
    _state.link.enable(enabled);
    return enabled;
}


int metronome::link_num_peers(){
    return _state.link.numPeers();
}

// Get quantum
double metronome::quantum(){
    return _quantum;
}

// Set quantum
double metronome::quantum(double quantum){
    _quantum = quantum;
    return _quantum;
}

// Get playing
bool metronome::playing(){
    return _state.link.captureAppSessionState().isPlaying();
}

// Set playing
bool metronome::playing(bool playing){
    auto sessionState = _state.link.captureAppSessionState();
    sessionState.setIsPlayingAndRequestBeatAtTime(true, now(), 0., _quantum);
    _state.link.commitAppSessionState(sessionState);
    return playing;
}


// Get tempo
double metronome::current_tempo(){
    auto sessionState = _state.link.captureAppSessionState();
    return sessionState.tempo();
}

// Set tempo
double metronome::current_tempo(double tempo){
    auto sessionState = _state.link.captureAppSessionState();
    sessionState.setTempo(tempo, now());
    _state.link.commitAppSessionState(sessionState);
    return tempo;
}

// Get beat
double metronome::link_beat(){
    auto sessionState = _state.link.captureAppSessionState();
    return sessionState.beatAtTime(now(), _quantum);
}

// Get phase
double metronome::phase(){
    auto sessionState = _state.link.captureAppSessionState();
    return sessionState.phaseAtTime(now(), _quantum);
}

void metronome::start() {

}
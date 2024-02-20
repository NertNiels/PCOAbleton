#include "metro.h"

double _quantum = 4.;

bool metronome::link_enabled() {
    return _link.isEnabled();
}

bool metronome::link_enabled(bool enabled) {
    _link.enable(enabled);
    return enabled;
}


int metronome::link_num_peers(){
    return _link.numPeers();
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
    return _link.captureAppSessionState().isPlaying();
}

// Set playing
bool metronome::playing(bool playing){
    auto sessionState = _link.captureAppSessionState();
    sessionState.setIsPlayingAndRequestBeatAtTime(true, now(), 0., _quantum);
    _link.commitAppSessionState(sessionState);
    return playing;
}


// Get tempo
double metronome::current_tempo(){
    auto sessionState = _link.captureAppSessionState();
    return sessionState.tempo();
}

// Set tempo
double metronome::current_tempo(double tempo){
    auto sessionState = _link.captureAppSessionState();
    sessionState.setTempo(tempo, now());
    _link.commitAppSessionState(sessionState);
    return tempo;
}

// Get beat
double metronome::link_beat(){
    auto sessionState = _link.captureAppSessionState();
    return sessionState.beatAtTime(now(), _quantum);
}

// Get phase
double metronome::phase(){
    auto sessionState = _link.captureAppSessionState();
    return sessionState.phaseAtTime(now(), _quantum);
}
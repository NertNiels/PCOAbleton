#include "metro.h"

double _quantum = 4.;

metronome::metronome() : _link(120.) {
    link_enabled(false);
    set_metro(this);
}

bool metronome::link_enabled() {
    return _link.isEnabled();
}

bool metronome::link_enabled(bool enabled) {
    _link.enable(enabled);
    return enabled;
}


int metronome::link_num_peers(){
    return static_cast<int>(_link.numPeers());
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
    if(playing) sessionState.setIsPlayingAndRequestBeatAtTime(playing, now(), 0., _quantum);
    else sessionState.setIsPlayingAndRequestBeatAtTime(playing, now(), 0, _quantum);
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

double metronome::link_beatAtTime(std::chrono::microseconds time){
    auto sessionState = _link.captureAppSessionState();
    return sessionState.beatAtTime(time, _quantum);
}

double metronome::beatAtTime(std::chrono::microseconds time){
    auto sessionState = _link.captureAppSessionState();
    return sessionState.beatAtTime(time, _quantum)*_beat_modifier;
}

// Get phase
double metronome::phase(){
    auto sessionState = _link.captureAppSessionState();
    return sessionState.phaseAtTime(now(), _quantum);
}

double metronome::link_phaseAtTime(std::chrono::microseconds time, double quantum) {
    auto sessionState = _link.captureAppSessionState();
    return sessionState.phaseAtTime(time, quantum);
}

double metronome::link_phaseAtTime(std::chrono::microseconds time) {
    return link_phaseAtTime(time, _quantum);
}

double metronome::phaseAtTime(std::chrono::microseconds time, double quantum) {
    auto sessionState = _link.captureAppSessionState();
    return sessionState.phaseAtTime(time, quantum/_beat_modifier)*_beat_modifier;
}

double metronome::phaseAtTime(std::chrono::microseconds time) {
    return phaseAtTime(time, _quantum);
}

double metronome::beat_modifier() {
    return _beat_modifier;
}

double metronome::beat_modifier(double mod) {
    _beat_modifier = mod;
    return mod;
}

double metronome::beat() {
    return link_beat()*_beat_modifier;
}


void callback_audio(const double& timeAtStart, const double& sampleAtStart, const size_t& numSamples, std::vector<double>* buffer, std::chrono::microseconds& latency) {
    if(!metronome::global_metro()->playing()) return;
    
    static double timeLastClick = 0;
    std::chrono::microseconds linkTimeLatency = metronome::global_metro()->now()+latency;

    const double highTone = 1500.;
    const double lowTone = 1000.;

    double hostTime = timeAtStart;
    const double microsPerSample = 1e6/44100.;
    const std::chrono::microseconds microsPerSample_std(llround(microsPerSample));

    for(size_t i = 0; i < numSamples; i++) {
        if(metronome::global_metro()->beatAtTime(linkTimeLatency) >= 0.) {
            double lastPhase = metronome::global_metro()->phaseAtTime(linkTimeLatency-microsPerSample_std, 1);
            double phase = metronome::global_metro()->phaseAtTime(linkTimeLatency, 1);
            if(phase < lastPhase) timeLastClick = hostTime;
            lastPhase = phase;
            double amplitude = 0;
            double tone = floor(metronome::global_metro()->phaseAtTime(linkTimeLatency))==0 ? highTone : lowTone;
            if(hostTime-timeLastClick < 100000) amplitude = cos(2.*M_PI*((hostTime)/1e6)*tone); // Cosine wave per microseconds interval
            (*buffer)[i] = amplitude;
        }
        hostTime += microsPerSample;
        linkTimeLatency += std::chrono::microseconds(llround(microsPerSample));
    }
}

metronome* metronome::_metro = nullptr;

metronome* metronome::global_metro() {
    return _metro;
}

void metronome::set_metro(metronome* metro) {
    _metro = metro;
}
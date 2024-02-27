#include "app.h"

void guiapp::setup() {
    pcoapi::authenticate();
    pco_organization = pcoapi::get_organization();
    metro.link_enabled(true);
    
    static audio_handler::audio_engine engine;
    engine.callback_metronome = &callback_audio;
}

void guiapp::update() {
    _setlist_ui.update_ui(pco_organization, metro);
}
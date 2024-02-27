#define PCOAPI_LOAD_ASYNC 1

#pragma once
#include <iostream>
#include <imgui.h>
#include <imgui_internal.h>
#include <pcoableton/metro.h>
#include "audio_handler.h"
#include "setlist.h"

class guiapp {
    
    private:
        pcoapi::organization pco_organization;
        metronome& metro;
        void setup();
        setlist_ui _setlist_ui;

    public:
        void update();
        guiapp::guiapp() : metro(*(new metronome())) { setup(); }

};
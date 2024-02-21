#define PCOAPI_LOAD_ASYNC 1

#include <iostream>
#include <imgui.h>
#include <imgui_internal.h>
#include <pcoableton/pcoapi.h>
#include <pcoableton/metro.h>
#include "audio_handler.h"


class guiapp {
    
    private:
        pcoapi::organization pco_organization;
        metronome& metro;
        void setup();

    public:
        void update();
        guiapp::guiapp() : metro(*(new metronome())) { setup(); }

};
#define PCOAPI_LOAD_ASYNC 1

#include <iostream>
#include <imgui.h>
#include <pcoableton/pcoapi.h>


namespace guiapp {
    
    static pcoapi::organization pco_organization;

    void setup();
    void update();
};
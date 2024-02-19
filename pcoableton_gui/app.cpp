#include "app.h"

void guiapp::setup() {
    pcoapi::authenticate();
    pco_organization = pcoapi::get_organization();
}

void guiapp::update() {
    ImGui::Begin("Choose service");
    
    ImGui::Text(("Hello " + pco_organization.person_first_name + " " + pco_organization.person_last_name + "!").c_str());

    static int t_id;
    static pcoapi::service_type *loaded_type;    
    for(int i = 0; i < pco_organization.service_types.size(); i++) {
        pcoapi::service_type *t = &pco_organization.service_types[i];
        if(ImGui::RadioButton(t->name.c_str(), &t_id, stoi(t->id))) {
            std::cout << t->name << std::endl;
            pcoapi::load_serviceplans(t);
            loaded_type = t;
        }
    }
    
    ImGui::End();

    ImGui::Begin("Choose plan");

    static int p_id;
    static pcoapi::service_plan *loaded_plan;    
    if(loaded_type != nullptr) {
        for(int i = 0; i < loaded_type->plans.size(); i++) {
            pcoapi::service_plan *p = &loaded_type->plans[i];
            if(ImGui::RadioButton(p->date.c_str(), &p_id, stoi(p->id))) {
                std::cout << p->date << std::endl;
                pcoapi::load_serviceplanitems(p);
                loaded_plan = p;
            }
        }
    }

    ImGui::End();

    ImGui::Begin("Choose item");

    static int i_id;
    static pcoapi::service_plan_item *opened_item;
    if(loaded_plan != nullptr) {
        for(int i = 0; i < loaded_plan->items.size(); i++) {
            pcoapi::service_plan_item *item = &loaded_plan->items[i];
            if(ImGui::RadioButton(item->title.c_str(), &i_id, stoi(item->id))) {
                std::cout << item->title << std::endl;
                opened_item = item;
            }
        }
    }

    ImGui::End();

    ImGui::Begin("Item");

    if(opened_item != nullptr) {
        static bool metronome_state;
        ImGui::Text(("Title: " + opened_item->title).c_str());
        ImGui::Text("BPM: %.1f", opened_item->bpm);
        ImGui::Text(("Meter: " + opened_item->meter).c_str());
        if(ImGui::Button("Metronome")) {
            metronome_state = !metronome_state;
            std::cout << "metronome on: " << metronome_state << std::endl;
        }
    }
    
    ImGui::End();
}
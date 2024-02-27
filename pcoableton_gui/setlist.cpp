#include "setlist.h"

// CLASS SETLIST

setlist::setlist() : _is_editable(true) { }

setlist::setlist(pcoapi::service_plan plan) {
    items = plan.items;
    _is_editable = false;
}

setlist::~setlist() { }

void setlist::add_song(std::string title, float bpm, std::string meter, std::string key) {
    if(!_is_editable) return;
    pcoapi::service_plan_item item;
    item.parent = "";
    item.title = title;
    item.song_id = "";
    item.arrangement_id = "";
    item.key = key;
    item.is_song = true;
    item.bpm = bpm;
    item.meter = meter;
    item.length = 0;

    items.push_back(item);
}
void setlist::add_song(std::string title, float bpm, std::string meter) {
    add_song(title, bpm, meter, "");
}


void setlist::add_song(pcoapi::service_plan_item item) {
    if(!_is_editable) return;
    items.push_back(item);
}

pcoapi::service_plan_item& setlist::get_item(int index) {
    return items[index];
}

std::vector<pcoapi::service_plan_item>& setlist::get_items() {
    return items;
}

// END CLASS SETLIST

// CLASS SETLIST UI

setlist_ui::setlist_ui() {
    _setlist = new setlist();
}
setlist_ui::~setlist_ui() { }

void setlist_ui::update_ui(pcoapi::organization& pco_organization) {
    ImGui::Begin("Setlist");
    if(ImGui::Button("Load") || _loading_ui) load_setlist_ui(pco_organization);

    for(int i = 0; i < _setlist->size(); i++) {
        ImGui::Text((*_setlist)[i].title.c_str());
    }

    ImGui::End();
}

void setlist_ui::load_setlist_ui(pcoapi::organization& pco_organization) {
    _loading_ui = true;

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
    static bool selected = false;

    if(loaded_plan != nullptr) {
        if(ImGui::Button("Load")) {
            selected = true;
            pcoapi::load_serviceplanitems(loaded_plan);
        }
        if(loaded_plan->loading) {
            ImGui::SameLine();
            ImGui::Text("Loading...");
        }
        if(selected && !loaded_plan->loading) {
            load_setlist(loaded_plan);
            selected = false;
        }
    }

    if(loaded_type != nullptr) {
        if(loaded_type->loading) ImGui::Text("Loading...");
        else {
            for(int i = 0; i < loaded_type->plans.size(); i++) {
                pcoapi::service_plan *p = &loaded_type->plans[i];
                if(ImGui::RadioButton(p->date.c_str(), &p_id, stoi(p->id))) {
                    std::cout << p->date << std::endl;
                    loaded_plan = p;
                    selected = false;
                }
            }
        }
    }

    ImGui::End();


}

void setlist_ui::load_setlist(pcoapi::service_plan* plan) {
    delete _setlist;
    _setlist = new setlist(*plan);
    _loading_ui = false;
}

// END CLASS SETLIST UI
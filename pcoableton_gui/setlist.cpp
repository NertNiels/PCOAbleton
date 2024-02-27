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

void setlist_ui::update_ui(pcoapi::organization& pco_organization, metronome& metro) {
    ImGui::Begin("Setlist");
    if(ImGui::Button("Load") || _loading_ui) load_setlist_ui(pco_organization);

    int i_id;
    for(int i = 0; i < _setlist->size(); i++) {
        if(ImGui::RadioButton((*_setlist)[i]->title.c_str(), &i_id, i)) {
            load_item((*_setlist)[i], metro);
        }
    }

    ImGui::End();

    ImGui::Begin("Item");

    if(item_ptr == nullptr) ImGui::Text("No item selected");
    else ImGui::Text(item_ptr->title.c_str());
    if(item_ptr != nullptr) {
        ImGui::Text("------");

        float tempo = (float)metro.current_tempo();
        ImGui::Text("Current tempo: ");
        ImGui::SameLine();
        if(ImGui::DragFloat(" ", &tempo, 1, 20., 999.)) metro.current_tempo((double)tempo);
        ImGui::Text("Current meter: %.1f", metro.quantum());
        auto beat = metro.beat();
        ImGui::Text("Current beat:  %.1f", beat);
    }

    ImGui::BeginTable("metronome", 2);
    ImGui::TableSetupColumn("AAA", ImGuiTableColumnFlags_WidthFixed, 50);
    ImGui::TableSetupColumn("BBB", ImGuiTableColumnFlags_WidthFixed, 50);
    ImGui::TableNextRow(ImGuiTableRowFlags_::ImGuiTableRowFlags_None, 50);
    if(metro.playing()) ImGui::TableSetBgColor(ImGuiTableBgTarget_::ImGuiTableBgTarget_CellBg, ImGui::GetColorU32(ImVec4(1.f, 1.f, 1.f, 1.0f)), (int)std::floor(std::fmod(metro.beat(), 2.)));
    ImGui::TableSetColumnIndex(0);
    ImGui::TableSetColumnIndex(1);
    ImGui::EndTable();

    if(ImGui::Button(metro.playing() ? "Playing" : "Stopped")) metro.playing(!metro.playing());
    ImGui::SameLine();
    if(ImGui::Button(":2")) {
        metro.beat_modifier(metro.beat_modifier()/2);
        if(item_ptr != nullptr) item_ptr->beat_modifier = metro.beat_modifier();
    }
    
    ImGui::SameLine();
    if(ImGui::Button("x2")) {
        metro.beat_modifier(metro.beat_modifier()*2);
        if(item_ptr != nullptr) item_ptr->beat_modifier = metro.beat_modifier();
    }

    ImGui::Text("------");

    if(ImGui::Button("Link")) metro.link_enabled(!metro.link_enabled());
    ImGui::SameLine();
    ImGui::Text(metro.link_enabled() ? "on" : "off");
    ImGui::Text("Peers in session: %i", metro.link_num_peers());

    ImGui::End();
}

void setlist_ui::load_setlist_ui(pcoapi::organization& pco_organization) {
    _loading_ui = true;

    ImGui::Begin("Choose service");
    
    static int t_id;
    static pcoapi::service_type *loaded_type;    
    for(int i = 0; i < pco_organization.service_types.size(); i++) {
        pcoapi::service_type *t = &pco_organization.service_types[i];
        if(ImGui::RadioButton(t->name.c_str(), &t_id, stoi(t->id))) {
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

void setlist_ui::load_item(pcoapi::service_plan_item* item, metronome& metro) {
    item_ptr = item;
    if(item->is_song) {
        metro.current_tempo(item_ptr->bpm);
        metro.quantum(stod(item_ptr->meter.substr(0,1)));
        metro.beat_modifier(item_ptr->beat_modifier);
    }
}

// END CLASS SETLIST UI
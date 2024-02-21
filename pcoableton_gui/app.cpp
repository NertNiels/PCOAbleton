#include "app.h"

void guiapp::setup() {
    pcoapi::authenticate();
    pco_organization = pcoapi::get_organization();
    metro.link_enabled(true);
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
        if(loaded_type->loading) ImGui::Text("Loading...");
        else {
            for(int i = 0; i < loaded_type->plans.size(); i++) {
                pcoapi::service_plan *p = &loaded_type->plans[i];
                if(ImGui::RadioButton(p->date.c_str(), &p_id, stoi(p->id))) {
                    std::cout << p->date << std::endl;
                    pcoapi::load_serviceplanitems(p);
                    loaded_plan = p;
                }
            }
        }
    }

    ImGui::End();

    ImGui::Begin("Choose item");

    static int i_id;
    static pcoapi::service_plan_item *opened_item;
    if(loaded_plan != nullptr) {
        if(loaded_plan->loading) ImGui::Text("Loading...");
        else {
            for(int i = 0; i < loaded_plan->items.size(); i++) {
                pcoapi::service_plan_item *item = &loaded_plan->items[i];
                if(ImGui::RadioButton(item->title.c_str(), &i_id, stoi(item->id))) {
                    std::cout << item->title << std::endl;
                    opened_item = item;
                }
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

    ImGui::Begin("Metronome");

    if(ImGui::Button("Link")) metro.link_enabled(!metro.link_enabled());
    ImGui::SameLine();
    ImGui::Text(metro.link_enabled() ? "on" : "off");
    ImGui::Text("Peers in session: %i", metro.link_num_peers());
    ImGui::Text("------");

    float tempo = (float)metro.current_tempo();
    ImGui::Text("Current tempo: ");
    ImGui::SameLine();
    if(ImGui::DragFloat(" ", &tempo, 1, 20., 999.)) metro.current_tempo((double)tempo);
    ImGui::Text("Current meter: %.1f", metro.quantum());
    auto beat = metro.link_beat();
    ImGui::Text("Current beat:  %.1f", beat);

    ImGui::BeginTable("metronome", 2);
    ImGui::TableSetupColumn("AAA", ImGuiTableColumnFlags_WidthFixed, 50);
    ImGui::TableSetupColumn("BBB", ImGuiTableColumnFlags_WidthFixed, 50);
    ImGui::TableNextRow(ImGuiTableRowFlags_::ImGuiTableRowFlags_None, 50);
    ImGui::TableSetBgColor(ImGuiTableBgTarget_::ImGuiTableBgTarget_CellBg, ImGui::GetColorU32(ImVec4(1.f, 1.f, 1.f, 1.0f)), (int)std::floor(std::fmod(beat, 2.)));
    ImGui::TableSetColumnIndex(0);
    ImGui::TableSetColumnIndex(1);
    ImGui::EndTable();

    ImGui::End();
}
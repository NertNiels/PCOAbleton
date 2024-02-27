#include "app.h"

void guiapp::setup() {
    pcoapi::authenticate();
    pco_organization = pcoapi::get_organization();
    metro.link_enabled(true);
    
    static audio_handler::audio_engine engine;
    engine.callback_metronome = &callback_audio;
}

void guiapp::update() {
    // ImGui::Begin("Choose item");

    // static int i_id;
    // static pcoapi::service_plan_item *opened_item;
    // if(loaded_plan != nullptr) {
    //     if(loaded_plan->loading) ImGui::Text("Loading...");
    //     else {
    //         for(int i = 0; i < loaded_plan->items.size(); i++) {
    //             pcoapi::service_plan_item *item = &loaded_plan->items[i];
    //             if(ImGui::RadioButton(item->title.c_str(), &i_id, stoi(item->id))) {
    //                 std::cout << item->title << std::endl;
    //                 opened_item = item;
    //             }
    //         }
    //     }
    // }

    // ImGui::End();

    // ImGui::Begin("Item");

    // if(opened_item != nullptr) {
    //     static bool metronome_state;
    //     ImGui::Text(("Title: " + opened_item->title).c_str());
    //     ImGui::Text("BPM: %.1f", opened_item->bpm);
    //     ImGui::Text(("Meter: " + opened_item->meter).c_str());
    //     if(ImGui::Button("Metronome")) {
    //         metro.current_tempo(opened_item->bpm);
    //         metro.quantum(stoi(opened_item->meter.substr(0, 1)));
    //     }
    // }
    
    // ImGui::End();

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

    if(ImGui::Button(metro.playing() ? "Playing" : "Stopped")) metro.playing(!metro.playing());

    ImGui::End();

    _setlist_ui.update_ui(pco_organization);
}
#pragma once
#include <pcoableton/pcoapi.h>
#include <iostream>
#include <imgui.h>

class setlist {

public:
    setlist();
    setlist(pcoapi::service_plan plan);
    ~setlist();

    void add_song(std::string title, float bpm, std::string meter, std::string key);
    void add_song(std::string title, float bpm, std::string meter);
    
    void add_song(pcoapi::service_plan_item item);
    pcoapi::service_plan_item& get_item(int index);
    std::vector<pcoapi::service_plan_item>& setlist::get_items();

    pcoapi::service_plan_item operator[](int i) {
        return items[i];
    }

    size_t size() {
        return items.size();
    }

private:
    bool _is_editable;
    std::vector<pcoapi::service_plan_item> items;
};

class setlist_ui {

public:
    setlist_ui();
    ~setlist_ui();

    void update_ui(pcoapi::organization& pco_organization);

private:
    setlist* _setlist = nullptr;

    void load_setlist(pcoapi::service_plan* plan);
    void load_setlist_ui(pcoapi::organization& pco_organization);
    bool _loading_ui = false;
};
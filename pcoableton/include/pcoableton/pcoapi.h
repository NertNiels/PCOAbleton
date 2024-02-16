#include <iostream>
#include <fstream>
#include <string>
#include <cpr/cpr.h>
#include <nlohmann/json.hpp>
#include <ctime>

namespace pcoapi {
    struct service_plan_item {
        std::string parent;
        std::string id;
        std::string title;
        std::string song_id;
        std::string arrangement_id;
        std::string key;
        bool is_song;
        int bpm;
        std::string meter;
        int length;
    };

    struct service_plan {
        std::string parent;
        std::string id;
        std::string title;
        std::string date;
        std::vector<service_plan_item> items;

        time_t last_api_update;
    };

    struct service_type {
        std::string id;
        std::string name;
        std::vector<service_plan> plans;

        time_t last_api_update;
    };

    void authenticate();
    std::vector<service_type> get_servicetypes();
    std::vector<service_plan> get_serviceplans(std::string servicetype, bool future, bool no_dates);
    std::vector<service_type> get_overview();
    std::vector<service_plan_item> get_serviceplanitems(std::string servicetype, std::string serviceplan);
    void get_arrangementinfo(std::string song, std::string arragement, int *bpm, std::string *meter);
    void load_serviceplans(service_type *servicetype, bool refresh=false);
    void load_serviceplanitems(service_plan *serviceplan, bool refresh=false);
}
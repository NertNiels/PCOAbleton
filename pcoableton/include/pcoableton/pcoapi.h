#define PCOAPI_LOAD_ASYNC 1

#include <iostream>
#include <fstream>
#include <string>
#include <cpr/cpr.h>
#include <nlohmann/json.hpp>
#include <ctime>
#include <future>

namespace pcoapi {
    struct service_plan_item {
        std::string parent;
        std::string id;
        std::string title;
        std::string song_id;
        std::string arrangement_id;
        std::string key;
        bool is_song;
        float bpm;
        std::string meter;
        int length;

        double beat_modifier = 1;
    };

    struct service_plan {
        bool loading = 0;

        std::string parent;
        std::string id;
        std::string title;
        std::string date;
        std::vector<service_plan_item> items;

        time_t last_api_update;
    };

    struct service_type {
        bool loading = 0;

        std::string id;
        std::string name;
        std::vector<service_plan> plans;

        time_t last_api_update;
    };

    struct organization {
        bool loading = 0;

        std::string person_first_name;
        std::string person_last_name;
        std::string organization_name;

        std::vector<service_type> service_types;

        service_type get_servicetype(int id) {
            for(int i = 0; i < service_types.size(); i++) {
                if(stoi(service_types[i].id)==id) return service_types[i];
            }
            service_type t;
            return t;
        };
    };

    void authenticate();
    std::vector<service_type> get_servicetypes();
    std::vector<service_plan> get_serviceplans(std::string servicetype, bool future, bool no_dates);
    std::vector<service_type> get_overview();
    std::vector<service_plan_item> get_serviceplanitems(std::string servicetype, std::string serviceplan);
    void get_arrangementinfo(std::string song, std::string arragement, float *bpm, std::string *meter);
    static void load_serviceplans_async(service_type *servicetype);
    void load_serviceplans(service_type *servicetype, bool refresh=false);
    void load_serviceplanitems(service_plan *serviceplan, bool refresh=false);
    organization get_organization();

    static std::mutex organization_mutex;
    static std::vector<std::future<void>> futures;
}
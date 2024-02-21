#include "pcoapi.h"

namespace pcoapi {
    std::string api_root = "https://api.planningcenteronline.com/api/v2";
    std::string api_services = "https://api.planningcenteronline.com/services/v2";
    std::string api_services_people = "https://api.planningcenteronline.com/services/v2/people";
    std::string api_services_songs = "https://api.planningcenteronline.com/services/v2/songs";
    std::string api_services_service_types = "https://api.planningcenteronline.com/services/v2/service_types";

    std::string app_id;
    std::string secret;

    nlohmann::json callapi(std::string url) {
        cpr::Response r = cpr::Get(cpr::Url{url},
                                cpr::Authentication{app_id, secret, cpr::AuthMode::BASIC});
                                //cpr::Parameters{{"anon", "true"}, {"key", "value"}});
        if(r.status_code != 200) {
            std::cerr << "API responded with error code: " << r.status_code << std::endl;
            std::cerr << r.raw_header << std::endl;
            std::cerr << r.text << std::endl;
            return nullptr;
        }
        nlohmann::json obj = nlohmann::json::parse(r.text);
        return obj;
    }

    void getauth() {
        std::ifstream f;
        f.open("C:\\Dev\\PCOAbleton\\auth.txt");
        if(errno!=0) {
            std::cerr << "Error while opening auth.txt: " << strerror(errno) << std::endl;
            return;
        }

        if(f.is_open()) {
            f >> app_id;
            f >> secret;
            f.close();
        }
    }

    void authenticate() {
        getauth();
        if(callapi(api_services)==nullptr) throw std::invalid_argument("Could not authenticate with the given APP_ID and SECRET. Please review the credentials.");
        else std::cout << "Authentication successful" << std::endl;
    }

    std::vector<service_type> get_overview() {
        std::vector<service_type> servicetypes = get_servicetypes();
        for(int i = 0; i < servicetypes.size(); i++) {
            servicetypes[i].plans = get_serviceplans(servicetypes[i].id, true, true);
            std::cout;
        }
        return servicetypes;
    }

    std::vector<service_type> get_servicetypes() {
        nlohmann::json obj = callapi(api_services_service_types);

        std::vector<service_type> a;
        for(auto it : obj["data"]) {
            service_type s;
            s.id = it["id"];
            s.name = (it["attributes"]["name"]==nullptr ? "No name" : it["attributes"]["name"]);
            s.last_api_update = time(0);
            a.push_back(s);
        }
        return a;
    }

    void sort_serviceplans(std::vector<service_plan> *a, nlohmann::json obj) {
        bool get = true;
        while(get) {
            for(auto it : obj["data"]) {
                service_plan p;
                p.id = it["id"];
                p.title = (it["attributes"]["title"]==nullptr ? "-" : it["attributes"]["title"]);
                p.date = (it["attributes"]["dates"]==nullptr ? "No dates" : it["attributes"]["dates"]);
                p.parent = (it["relationships"]["service_type"]["data"]["id"]);
                p.last_api_update = time(0);
                a->push_back(p);
            }
            if(obj["links"]["next"]==nullptr) get=false;
            else obj = callapi(obj["links"]["next"]);
        }
    }

    std::vector<service_plan> get_serviceplans(std::string servicetype, bool future, bool no_dates) {
        nlohmann::json obj = callapi(api_services_service_types + "/" + servicetype + "/plans" + (future ? "?filter=future" : ""));
        std::vector<service_plan> a;
        sort_serviceplans(&a, obj);
        if(no_dates) obj = callapi(api_services_service_types + "/" + servicetype + "/plans?filter=no_dates");
        sort_serviceplans(&a, obj);
        return a;
    }

    void sort_serviceplanitems(std::vector<service_plan_item> *a, nlohmann::json obj) {
        bool get = true;
        while(get) {
            for(auto it : obj["data"]) {
                service_plan_item i;
                i.id = it["id"];
                i.title = (it["attributes"]["title"]==nullptr ? "-" : it["attributes"]["title"]);
                i.length = (it["attributes"]["length"]==nullptr ? 0 : it["attributes"]["length"]);
                i.parent = (it["relationships"]["plan"]["data"]["id"]);
                i.is_song = it["attributes"]["item_type"]=="song";
                if(i.is_song) {
                    i.song_id = (it["relationships"]["song"]["data"]["id"]==nullptr ? "" : it["relationships"]["song"]["data"]["id"]);
                    i.arrangement_id = (it["relationships"]["arrangement"]["data"]["id"]==nullptr ? "" : it["relationships"]["arrangement"]["data"]["id"]);
                    i.key = (it["attributes"]["key_name"]==nullptr ? "" : it["attributes"]["key_name"]);
                    get_arrangementinfo(i.song_id, i.arrangement_id, &i.bpm, &i.meter);
                }
                a->push_back(i);
            }
            if(obj["links"]["next"]==nullptr) get=false;
            else obj = callapi(obj["links"]["next"]);
        }
    }

    std::vector<service_plan_item> get_serviceplanitems(std::string servicetype, std::string serviceplan) {
        nlohmann::json obj = callapi(api_services_service_types + "/" + servicetype + "/plans/" + serviceplan + "/items");
        std::vector<service_plan_item> a;
        sort_serviceplanitems(&a, obj);
        return a;
    }

    void get_arrangementinfo(std::string song, std::string arrangement, float *bpm, std::string *meter) {
        nlohmann::json obj = callapi(api_services_songs+"/"+song+"/arrangements/"+arrangement);
        std::cout << "DEBUG: " << obj << std::endl;
        *bpm = (obj["data"]["attributes"]["bpm"] == nullptr ? .0f : obj["data"]["attributes"]["bpm"]);
        *meter = (obj["data"]["attributes"]["meter"] == nullptr ? "" : obj["data"]["attributes"]["meter"]);
    }

    static void load_serviceplans_async(service_type *servicetype) {
        std::vector<service_plan> plans = get_serviceplans(servicetype->id, true, true);

        std::lock_guard<std::mutex> lock(organization_mutex);
        servicetype->plans = plans;
        servicetype->last_api_update = time(0);
        servicetype->loading = false;
    }

    static void load_serviceplanitems_async(service_plan *serviceplan) {
        std::vector<service_plan_item> items = get_serviceplanitems(serviceplan->parent, serviceplan->id);

        std::lock_guard<std::mutex> lock(organization_mutex);
        serviceplan->items = items;
        serviceplan->last_api_update = time(0);
        serviceplan->loading = false;
    }

    void load_serviceplans(service_type *servicetype, bool refresh) {
#if PCOAPI_LOAD_ASYNC
        if(servicetype->plans.size()==0||refresh) {
            servicetype->loading = true;
            futures.push_back(std::async(std::launch::async, load_serviceplans_async, servicetype));
        }
        std::cout << "help" << std::endl;
#else
        if(servicetype->plans.size()==0||refresh) {
            servicetype->plans = get_serviceplans(servicetype->id, true, true);
            servicetype->last_api_update = time(0);
        }
#endif
    }

    void load_serviceplanitems(service_plan *serviceplan, bool refresh) {
#if PCOAPI_LOAD_ASYNC
        if(serviceplan->items.size()==0||refresh) {
            serviceplan->loading = true;
            futures.push_back(std::async(std::launch::async, load_serviceplanitems_async, serviceplan));
        }
#else
        if(serviceplan->items.size()==0||refresh) {
            serviceplan->items = get_serviceplanitems(serviceplan->parent, serviceplan->id);
            serviceplan->last_api_update = time(0);
        }
#endif
    }

    organization get_organization() {
        // nlohmann::json obj = callapi(api_root);
        organization o;
        // o.person_first_name = (obj["attributes"]["first_name"]==nullptr ? "-" : obj["attributes"]["first_name"]);
        // o.person_last_name = (obj["attributes"]["last_name"]==nullptr ? "-" : obj["attributes"]["last_name"]);
        o.service_types = get_servicetypes();

        return o;
    }
}
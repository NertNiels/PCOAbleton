#include "pcoapi.h"

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
        return nullptr;
    }
    nlohmann::json obj = nlohmann::json::parse(r.text);
    std::cout << "API succesful" << std::endl;
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

void pcoapi() {
    getauth();

    std::vector<service_type> servicetypes = get_servicetypes();
    for(int i = 0; i < servicetypes.size(); i++) {
        servicetypes[i].plans = get_serviceplans(servicetypes[i].id, true, true);
        std::cout;
    }

    for(service_type a : servicetypes) {
        std::cout << a.name << std::endl;
        for(service_plan it : a.plans) {
            std::cout << "\t- Date: " << it.date << ", ID: " << it.id << ", Title: " << it.title << std::endl;
        }
    }
}

std::vector<service_type> get_servicetypes() {
    nlohmann::json obj = callapi(api_services_service_types);

    std::vector<service_type> a;
    for(auto it : obj["data"]) {
        service_type s;
        s.id = it["id"];
        s.name = (it["attributes"]["name"]==nullptr ? "No name" : it["attributes"]["name"]);
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
            a->push_back(p);
        }
        if(obj["links"]["next"]==nullptr) get=false;
        else obj = callapi(obj["links"]["next"]);
    }
}

std::vector<service_plan> get_serviceplans(std::string servicetype, bool future, bool no_dates) {
    nlohmann::json obj = callapi(api_services_service_types + "/" + servicetype + "/plans" + (future ? "?filter=future" : ""));
    std::vector<service_plan> a;
    bool get = true;
    // while(get) {
    //     for(auto it : obj["data"]) {
    //         service_plan p;
    //         p.id = it["id"];
    //         p.title = (it["attributes"]["title"]==nullptr ? "-" : it["attributes"]["title"]);
    //         p.date = (it["attributes"]["dates"]==nullptr ? "No dates" : it["attributes"]["dates"]);
    //         a.push_back(p);
    //     }
    //     if(obj["links"]["next"]==nullptr) get = false;
    //     else obj = callapi(obj["links"]["next"]);
    // }
    sort_serviceplans(&a, obj);
    get = no_dates;
    if(get) obj = callapi(api_services_service_types + "/" + servicetype + "/plans?filter=no_dates");
    sort_serviceplans(&a, obj);
    return a;
}
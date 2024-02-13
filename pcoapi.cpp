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
    service_type* a = get_servicetypes();
}

service_type* get_servicetypes() {
    nlohmann::json obj = callapi(api_services_service_types);

    service_type *a{ new service_type[obj["data"].size()]{} }; 
    int i = 0;
    for(auto it : obj["data"]) {

        a[i].id = it["id"];
        a[i].name = it["attributes"]["name"];
        i++;
    }
    return a;
}
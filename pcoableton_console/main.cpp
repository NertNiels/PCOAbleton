#include "main.h"

metronome metro;

int main() {

    std::cout << "hallo" << std::endl;
    pcoapi::authenticate();

    service_types = pcoapi::get_servicetypes();
    print_overview();
    while(true) {
        handle_input();
        print_overview();
    }
    return 0;
}

void handle_input() {
    std::string c, d, e, f;
    std::cout << "pcoableton>";
    std::cin >> c;

    if(c == "loadplans") {
        std::cout << "ServiceType ID: ";
        std::cin >> c;
        for(int i=0; i<service_types.size(); i++) {
            if(service_types[i].id!=c) continue;
            pcoapi::load_serviceplans(&service_types[i]);
        }
    }
    else if(c == "loaditems") {
        std::cout << "ServiceType ID: ";
        std::cin >> c;
        std::cout << "Plan ID: ";
        std::cin >> d;
        for(int i=0; i<service_types.size(); i++) {
            if(service_types[i].id!=c) continue;
            for(int j=0; j<service_types[i].plans.size(); j++) {
                if(service_types[i].plans[j].id!=d) continue;
                pcoapi::load_serviceplanitems(&service_types[i].plans[j]);
            }
        }
    } else if(c == "link") {
        std::cout << "Current state of LINK:" << std::endl;
        std::cout << "Peers connected: " << metro.link_num_peers() << std::endl;
        std::cout << "Link enabled: " << metro.link_enabled() << std::endl;
        std::cout << "Playing: " << metro.playing() << std::endl;
        std::cout << "Current tempo: " << metro.current_tempo() << std::endl;
        std::cout << "Current quantum: " << metro.quantum() << std::endl;
        std::cout << "Current ceat: " << metro.link_beat() << std::endl;
    }
    else {
        std::cout << "Command \"" << c << "\" unknown..." << std::endl;
    }
}

void print_servicetypes() {
    for(pcoapi::service_type a : service_types) {
        std::cout << a.name << ": " << a.id << std::endl;
        for(pcoapi::service_plan it : a.plans) {
            std::cout << "\t- Date: " << it.date << ", ID: " << it.id << ", Title: " << it.title << std::endl;
        }
    }
}

void print_serviceplanitems(std::string stype, std::string p) {
    for(pcoapi::service_type a : service_types) {
        if(a.id!=stype) continue;
        std::cout << a.name << ": " << a.id << std::endl;
        for(pcoapi::service_plan it : a.plans) {
            if(it.id!=p) continue;
            std::cout << "\t- Date: " << it.date << ", ID: " << it.id << ", Title: " << it.title << std::endl;
            for(pcoapi::service_plan_item pi : it.items) {
                std::cout << "\t\t- Title: " << pi.title << ", Length: " << pi.length << ", ID: " << pi.id << std::endl;\
            }
        }
    }
}

void print_overview() {
    std::cout << "------------------------------------------" << std::endl;
    for(pcoapi::service_type a : service_types) {
        std::cout << a.name << ": " << a.id << ", Last update: " << ctime(&a.last_api_update);
        for(pcoapi::service_plan it : a.plans) {
            std::cout << "\t- Date: " << it.date << ", ID: " << it.id << ", Title: " << it.title << ", Last update: " << ctime(&it.last_api_update);
            for(pcoapi::service_plan_item pi : it.items) {
                std::cout << "\t\t- Title: " << pi.title << ", Length: " << pi.length << ", ID: " << pi.id << std::endl;\
            }
        }
    }
}
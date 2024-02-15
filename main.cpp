#include "main.h"

int main() {
    gui();

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
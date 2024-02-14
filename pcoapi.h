#include <iostream>
#include <fstream>
#include <string>
#include <cpr/cpr.h>
#include <nlohmann/json.hpp>


struct service_plan {
    std::string id;
    std::string title;
    std::string date;
};

struct service_type {
    std::string id;
    std::string name;
    std::vector<service_plan> plans;
};


void getauth();
void pcoapi();
std::vector<service_type> get_servicetypes();
std::vector<service_plan> get_serviceplans(std::string servicetype, bool future, bool no_dates);
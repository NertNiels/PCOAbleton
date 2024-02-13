#include <iostream>
#include <fstream>
#include <string>
#include <cpr/cpr.h>
#include <nlohmann/json.hpp>

struct service_type {
    std::string id;
    std::string name;
};

void getauth();
void pcoapi();
service_type* get_servicetypes();


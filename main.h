#include <iostream>
#include "pcoapi.h"

void handle_input();

std::vector<pcoapi::service_type> service_types;
void print_servicetypes();
void print_serviceplanitems(std::string stype, std::string p);
void print_overview();
#include <iostream>
#include <string>
#include <curl/curl.h>
#include "json.hpp"
#include "api.h"
#include "struct/car.h"

using js = nlohmann::json;

size_t Wcallb(void* contents, size_t size, size_t nmemb, std::string* s) {
    s->append((char*)contents, size * nmemb);
    return size * nmemb;
}

void Recivedata::getCar() {
   
}
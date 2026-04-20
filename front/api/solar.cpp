#include <curl/curl.h>
#include "json.hpp"
#include "api.h"
#include "../struct/solar.h"

using js = nlohmann::json;
Solar s;

size_t Wcallb(void* contents, size_t size, size_t nmemb, std::string* s) {
    s->append((char*)contents, size * nmemb);
    return size * nmemb;
}

Solar Recivedata::getSolar() {

    CURL* curl;
    std::string res;

    curl  =curl_easy_init();

    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:8080/Getsolar");
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, Wcallb);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &res);

        curl_easy_perform(curl);
        curl_easy_cleanup(curl);
    }

    js j = js::parse(res);
    s.Ptotal = j["Ptotal"].get<double>();
    s.Ptransfer = j["Ptransfer"].get<double>();

    return s;
    

}
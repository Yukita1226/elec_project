#include <libwebsockets.h>
#include "json.hpp"
#include "api.h"
#include "../struct/car.h"
#include <thread>

using js = nlohmann::json;

static Car c;
static bool g_ready = false;

static int ws_call(struct lws* wsi,enum lws_callback_reasons reason, void* user,void* in,size_t len)
{
    if (reason == LWS_CALLBACK_CLIENT_RECEIVE && in && len > 0)
    {
        try
        {
            {
                auto j = js::parse((char*)in, (char*)in + len);

                c.Ptotal      = j["ptotalC"];
                c.Ptransfer   = j["ptransferC"];

                g_ready = true;
            }
        } catch (...) {}
    }
    return 0;
      
}

static lws_protocols protocols[] = 
{
    { "energy", ws_call, 0, 4096 },
    { NULL, NULL, 0, 0 }

};

static void ws_s()
{

    lws_context_creation_info info{};

    info.port = CONTEXT_PORT_NO_LISTEN;
    info.protocols = protocols;

    lws_context* ctx = lws_create_context(&info);

    lws_client_connect_info cc{};
    cc.context = ctx;
    cc.address = "127.0.0.1";
    cc.port = 8080;
    cc.path = "/Getcar";
    cc.host = "127.0.0.1";
    cc.protocol = protocols[0].name;

    lws_client_connect_via_info(&cc);

    std::thread([ctx]()
    {

        while(1) lws_service(ctx,50);

    }).detach();

}

Car Recivedata::getCar()
{
    static bool s = false;

    if(!s)
    {

        ws_s();
        s = true;

    }
    return c;
}


#pragma once
//#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "httplib.h"
using namespace httplib;
Server svr;

#include "../DB/DBHandler.hpp"
class DBHandler;

class HttpHandler {
    public:
        DBHandler *dh;
        HttpHandler(DBHandler *dhs)
        {
            dh = dhs;
        }
        int Server_start();
        int Server_stop();
};

int HttpHandler::Server_start()
{
    svr.set_error_handler([](const auto& req, auto& res) {
        auto fmt = "<p>Error Status: <span style='color:red;'>%d</span></p>";
        char buf[BUFSIZ];
        snprintf(buf, sizeof(buf), fmt, res.status);
        res.set_content(buf, "text/html");
    });

    svr.Get("/hi", [](const Request& req, Response& res) {
        res.set_content("Hello World!", "text/plain");
    });

    svr.Get("/stop", [&](const Request& req, Response& res) {
        svr.stop();
    });


    // svr.Get("/dh", [&](const Request& req, Response& res) {
    //     std::vector<std::tuple<int, std::string> > result;
    //     result = this->dh.sql_execute("test","SELECT WHERE(*=*)");
    //     res.set_content("Hello World!", "text/plain");
    // });
    
    svr.listen("0.0.0.0", 5060);
    return 0;
}

int HttpHandler::Server_stop()
{
    svr.stop();
    return 0;
}
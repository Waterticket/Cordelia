// Project Cordelia 1.0
// Made by KYS

#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <array>
#include <string>
#include <fstream>
#include <chrono>

#include "framework/System/FxException.hpp"
#include "framework/System/StringToken.hpp"
#include "framework/System/FileHandler.hpp"
#include "framework/Modules/NginxSetting.hpp"
#include "framework/DB/DBHandler.hpp"
#include "framework/Web/httplib.h"

using std::cout;
using std::endl;
using std::cin;
using namespace httplib;
Server svr;

std::string exec(const char* cmd);

int main()
{
    DBHandler dh = DBHandler();
    dh.db_open("dbtest.csv", "test");
    NginxSetting ns = NginxSetting();

    std::string gss = "<form action=\"/post\" method=\"post\">"
    "First name: <input type=\"text\" name=\"k\"><br>"
    "<input type=\"submit\" value=\"Submit\">"
    "</form>";

    svr.Get("/hi", [](const Request& req, Response& res) {
        res.set_content("Hello World!", "text/plain");
    });

    svr.Get(R"(/numbers/(\d+))", [&](const Request& req, Response& res) {
        auto numbers = req.matches[1];
        res.set_content(numbers, "text/plain");
    });

    svr.Get("/stop", [&](const Request& req, Response& res) {
        svr.stop();
    });

    svr.Get("/send_post", [&](const Request& req, Response& res) {
        res.set_content(gss, "text/html");
    });

    svr.Post("/post", [](const Request &req, Response &res) {
        cout << req.get_param_value("k");
        res.set_content("asdf"+req.get_param_value("k"), "text/plain");
    });

    svr.listen("0.0.0.0", 5060);
}

std::string exec(const char* cmd) {
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
        std::cout << buffer.data();
    }
    return result;
}
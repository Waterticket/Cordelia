#pragma once
class HttpHandler;

svr.Get("/hello", [](const Request& req, Response& res) {
        res.set_content("Hello World!", "text/plain");
});
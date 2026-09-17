#include <cpp-httplib/httplib.h>
#include <thread>

#include "Backend.h"

int main()
{

    httplib::Server http_server;
    Backend backend;

    if(!http_server.set_mount_point("/", "/home/adi/ws/DragonFly/software/src/hmi/frontend/dist/dragonfly-hmi/browser"))
    {
        throw std::runtime_error("Failed to mount static site");
    }

    http_server.Get("/flight_controller_state_overview", [&](const httplib::Request& req, httplib::Response& res) {
        
        (void)req;

        std::optional<nlohmann::json> json = backend.getFlightControllerStateOverview();
        if(json)
        {
            res.set_content(json->dump(), "application/json");
        }
    });

    std::thread http_thread([&]()
    {
        http_server.listen("localhost", 1234);
    });

    while(true)
    {
        backend.tick();

        using namespace std::chrono_literals;
        std::this_thread::sleep_for(10ms);
    }
}
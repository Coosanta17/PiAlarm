#ifndef API_H
#define API_H

#include <httplib.h>
#include <vector>
#include <thread>
#include <memory>
#include "alarm.h"


class ApiServer {
    std::unique_ptr<httplib::Server> server;
    std::thread server_thread;
    bool server_running;
    int port;

    void registerEndpoints() const;

    static void addCorsHeaders(httplib::Response& res);

public:
    explicit ApiServer(int server_port = 8080);

    ~ApiServer();

    void start();

    void stop();

    [[nodiscard]] bool isRunning() const;
};

extern std::unique_ptr<ApiServer> g_apiServer;

void startApiServer(int port = 8080);

void stopApiServer();

#endif //API_H

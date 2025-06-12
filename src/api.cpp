#include "api.h"

#include <iostream>
#include <nlohmann/json.hpp>
#include "util.h"
#include "alarms_vector.h"

#define ALARM_MANAGER_WRITE_ACCESS

std::unique_ptr<ApiServer> g_apiServer = nullptr;

ApiServer::ApiServer(const int server_port)
    : server(nullptr), server_running(false), port(server_port) {
}

ApiServer::~ApiServer() {
    stop();
}

void ApiServer::start() {
    if (server_running) {
        std::cout << "API server is already running" << std::endl;
        return;
    }

    try {
        server = std::make_unique<httplib::Server>();

        server->Get("/v1/alarms", [](const httplib::Request &, httplib::Response &res) {
            try {
                nlohmann::json jsonResponse;
                toJson(jsonResponse["alarms"], AlarmsVector::getInstance().getAlarmsCopy());
                res.set_content(jsonResponse.dump(), "application/json");
            } catch (const std::exception &e) {
                res.status = 500;
                res.set_content(R"({"error":"Internal server error"})", "application/json");
                std::cerr << "Error processing request: " << e.what() << std::endl;
            }
        });

        // TODO: Add endpoints for creating, updating, deleting

        server_running = true;
        server_thread = std::thread([this]() {
            std::cout << "API server started on port " << port << std::endl;
            if (!server->listen("0.0.0.0", port)) {
                std::cerr << "Failed to start server on port " << port << std::endl;
                server_running = false;
            }
        });
    } catch (const std::exception &e) {
        throw std::runtime_error("Failed to start API server: " + std::string(e.what()));
    }
}

void ApiServer::stop() {
    if (!server_running) {
        return;
    }

    std::cout << "Stopping API server..." << std::endl;
    server->stop();

    if (server_thread.joinable()) {
        server_thread.join();
    }

    server_running = false;
    std::cout << "API server stopped" << std::endl;
}

bool ApiServer::isRunning() const {
    return server_running;
}

void startApiServer(int port) {
    if (!g_apiServer) {
        g_apiServer = std::make_unique<ApiServer>(port);
    }

    g_apiServer->start();
}

void stopApiServer() {
    if (g_apiServer) {
        g_apiServer->stop();
    }
}

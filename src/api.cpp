#include "api.h"

#include <iostream>
#include <nlohmann/json.hpp>
#include "util.h"
#include "alarms_vector.h"

std::unique_ptr<ApiServer> g_apiServer = nullptr;

ApiServer::ApiServer(const int server_port)
    : server(nullptr), server_running(false), port(server_port) {
}

ApiServer::~ApiServer() {
    stop();
}

void ApiServer::registerEndpoints() const {
    auto handleError = [
            ](httplib::Response &res, const int status, const std::string &message, const std::exception &e) {
        res.status = status;
        res.set_content(R"({"error":")" + message + R"("})", "application/json");
        std::cerr << "Error processing request: " << e.what() << std::endl;
    };

    auto validateAlarmIndex = [](const int index, const std::vector<Alarm> &alarms, httplib::Response &res) -> bool {
        if (index < 0 || static_cast<size_t>(index) >= alarms.size()) {
            res.status = 404;
            res.set_content(R"({"error":"Alarm not found - Index out of bounds."})", "application/json");
            return false;
        }
        return true;
    };

    auto sendSuccessResponse = [](httplib::Response &res, const int status, const std::string &message) {
        res.status = status;
        res.set_content(R"({"status":"success","message":")" + message + R"("})", "application/json");
    };

    /**
     * Example curl command:
     * curl -X GET http://localhost:8080/v1/alarms
     */
    // Get all alarms
    server->Get("/v1/alarms", [handleError](const httplib::Request &, httplib::Response &res) {
        try {
            nlohmann::json response;
            toJson(response["alarms"], AlarmsVector::getInstance().getAlarmsCopy());
            res.set_content(response.dump(), "application/json");
        } catch (const std::exception &e) {
            handleError(res, 500, "Internal server error", e);
        }
    });

    /**
     * Example curl command:
     * curl -X GET http://localhost:8080/v1/alarms/0
     */
    // Get alarm by index
    server->Get("/v1/alarms/([0-9]+)",
                [handleError, validateAlarmIndex](const httplib::Request &req, httplib::Response &res) {
                    try {
                        const int index = std::stoi(req.matches[1]);
                        const auto alarms = AlarmsVector::getInstance().getAlarmsCopy();

                        if (!validateAlarmIndex(index, alarms, res)) return;

                        res.set_content(alarms[index].toJson().dump(), "application/json");
                    } catch (const std::exception &e) {
                        handleError(res, 500, "Internal server error", e);
                    }
                });

    /**
     * Example curl command:
       curl -X POST http://localhost:8080/v1/alarms \
            -H "Content-Type: application/json" \
            -d '{"hour":21,"minute":0,"days":[0,1,2,3,4,5,6],"enabled":true}'
     */
    // Create new alarm
    server->Post("/v1/alarms", [handleError, sendSuccessResponse](const httplib::Request &req, httplib::Response &res) {
        try {
            const auto json = nlohmann::json::parse(req.body);
            AlarmsVector::getInstance().addAlarm(Alarm::createFromJson(json));
            AlarmsVector::getInstance().saveToFile();
            sendSuccessResponse(res, 201, "Alarm created");
        } catch (const nlohmann::json::parse_error &e) {
            handleError(res, 400, "Invalid JSON format", e);
        } catch (const std::exception &e) {
            handleError(res, 500, "Internal server error", e);
        }
    });

    /**
     * Example curl command:
       curl -X PUT http://localhost:8080/v1/alarms/0 \
            -H "Content-Type: application/json" \
            -d '{"hour":8,"minute":0,"days":[1,2,3,4,5],"enabled":true}'
     */
    // Update alarm at index
    server->Put("/v1/alarms/([0-9]+)",
                [handleError, validateAlarmIndex, sendSuccessResponse](const httplib::Request &req,
                                                                       httplib::Response &res) {
                    try {
                        const int index = std::stoi(req.matches[1]);

                        if (const auto alarms = AlarmsVector::getInstance().getAlarmsCopy();
                            !validateAlarmIndex(index, alarms, res)) { return; }

                        const auto json = nlohmann::json::parse(req.body);
                        AlarmsVector::getInstance().updateAlarm(index, Alarm::createFromJson(json));
                        AlarmsVector::getInstance().saveToFile();
                        sendSuccessResponse(res, 200, "Alarm updated");
                    } catch (const nlohmann::json::parse_error &e) {
                        handleError(res, 400, "Invalid JSON format", e);
                    } catch (const std::exception &e) {
                        handleError(res, 500, "Internal server error", e);
                    }
                });

    /**
     * Example curl command:
     * curl -X DELETE http://localhost:8080/v1/alarms/0
     */
    // Delete alarm at index
    server->Delete("/v1/alarms/([0-9]+)",
                   [handleError, validateAlarmIndex, sendSuccessResponse](
               const httplib::Request &req, httplib::Response &res) {
                       try {
                           const int index = std::stoi(req.matches[1]);

                           if (const auto alarms = AlarmsVector::getInstance().getAlarmsCopy();
                               !validateAlarmIndex(index, alarms, res)) { return; }

                           AlarmsVector::getInstance().deleteAlarm(index);
                           AlarmsVector::getInstance().saveToFile();
                           sendSuccessResponse(res, 200, "Alarm deleted");
                       } catch (const std::exception &e) {
                           handleError(res, 500, "Internal server error", e);
                       }
                   });
}

void ApiServer::start() {
    if (server_running) {
        std::cout << "API server is already running" << std::endl;
        return;
    }

    try {
        server = std::make_unique<httplib::Server>();

        registerEndpoints();

        // Start server in a separate thread
        server_running = true;
        server_thread = std::thread([this] {
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

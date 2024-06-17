#ifndef GODOT_XUMES_COMMUNICATION_H
#define GODOT_XUMES_COMMUNICATION_H

#include <iostream>
#include <memory>
#include <string>
#include <mutex>
#include "json.hpp"
#include "scene/main/node.h"
#include "core/object/object.h"
#include "scenario_runner.h"

#define CPPHTTPLIB_NO_EXCEPTIONS

#include "httplib.h"
#include "core/string/print_string.h"


using json = nlohmann::json;

class XumesCommunication {
public:
	static XumesCommunication* get_instance() {
		if (!singleton) {
			singleton = new XumesCommunication();
		}
		return singleton;
	}

	void run(int port) {
		if (!server_running) {
			server_running = true;
			server_thread = std::thread([this, port]() {
				server_.listen("0.0.0.0", port);
			});
		}
		print_line("HTTP Server started on ", port);
	}

	void close() {
		if (server_running) {
			print_line("Closing HTTP Server");
			server_.stop();
			if (server_thread.joinable()) {
				server_thread.join();
			}
			server_running = false;
		}
	}

private:
	XumesCommunication() : server_(), server_running(false) {
		setup_endpoints();
	}

	~XumesCommunication() {
		close();
	}

	void setup_endpoints() {
		server_.Post(R"(/start_scenarios/)", [&](const httplib::Request& req, httplib::Response& res) {
			auto body_json = json::parse(req.body, nullptr, false);

			if (body_json.is_discarded()) {
				res.status = 400; // Bad Request
				res.set_content("Invalid JSON format", "text/plain");
				return;
			}

			if (!body_json.empty()) {
				json response_json = json::object();
				for (const auto& scenario : body_json) {
					if (scenario.contains("methods") && scenario["methods"][0].contains("function_name") && scenario["methods"][0].contains("parameters")) {
						std::string function_name = scenario["methods"][0]["function_name"];

						if (function_name == "set_scene") {
							std::string arg;
							if (scenario["methods"][0]["parameters"].contains("args") && !scenario["methods"][0]["parameters"]["args"].empty()) {
								arg = scenario["methods"][0]["parameters"]["args"][0];
							} else {
								res.status = 400; // Bad Request
								res.set_content("Missing 'args' parameter", "text/plain");
								return;
							}

							String scenario_name = String(arg.c_str());
							int fps_limit = int(scenario["fps_limit"]);
							bool headless = !bool(scenario["render"]);
							OS::ProcessID pid = 0;
							ScenarioRunner::get_instance()->run(scenario_name, fps_limit, headless, &pid);
							uint16_t port = ScenarioRunner::get_instance()->get_port(pid);

							std::string scenario_name_str = scenario["name"].get<std::string>();
							response_json[scenario_name_str] = std::make_tuple(port, pid);
						} else {
							res.status = 400; // Bad Request
							res.set_content("Invalid function_name", "text/plain");
							return;
						}
					} else {
						res.status = 400; // Bad Request
						res.set_content("Invalid JSON format", "text/plain");
						return;
					}
				}
				res.status = 200; // OK
				res.set_content(response_json.dump(), "application/json");
			} else {
				res.status = 400; // Bad Request
				res.set_content("Empty JSON", "text/plain");
			}
		});

		server_.Post(R"(/stop_scenarios/)", [&](const httplib::Request& req, httplib::Response& res) {
			auto body_json = json::parse(req.body, nullptr, false);
			if (body_json.is_discarded()) {
				res.status = 400; // Bad Request
				res.set_content("Invalid JSON format", "text/plain");
				return;
			}
			if (!body_json.empty()) {
				json response_json = json::object();
				for (const auto& pid : body_json) {
					if (pid.is_number()) {
						uint16_t pid_value = pid.get<uint16_t>();
						if (pid_value != 0) {
							ScenarioRunner::get_instance()->stop(pid_value);
							response_json[std::to_string(pid_value)] = "stopped";
						} else {
							response_json[std::to_string(pid_value)] = "stop failed";
						}
					} else {
						res.status = 400; // Bad Request
						res.set_content("Invalid port value", "text/plain");
						return;
					}
				}
				res.status = 200; // OK
				res.set_content(response_json.dump(), "application/json");
			} else {
				res.status = 400; // Bad Request
				res.set_content("Empty JSON", "text/plain");
			}
		});
	}

	static XumesCommunication* singleton;
	static pid_t singleton_pid;
	static std::mutex instance_mutex;

	httplib::Server server_;
	bool server_running;
	std::thread server_thread;
};

XumesCommunication* XumesCommunication::singleton = nullptr;
pid_t XumesCommunication::singleton_pid = 0;
std::mutex XumesCommunication::instance_mutex;

#endif // GODOT_XUMES_COMMUNICATION_H

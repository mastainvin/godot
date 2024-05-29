//
// Created by vincent on 08/05/24.
//

#ifndef GODOT_XUMES_COMMUNICATION_H
#define GODOT_XUMES_COMMUNICATION_H

#include <iostream>
#include <memory>
#include <string>
#include "json.hpp" // Utilisation de nlohmann/json

#include "scene/main/node.h"
#include "core/object/object.h"
#include "scenario_runner.h"

#define CPPHTTPLIB_NO_EXCEPTIONS
#include "httplib.h"
#include <cstdlib> // Pour utiliser la fonction std::rand() pour générer un port aléatoire
#include "core/string/print_string.h"

using json = nlohmann::json;

class GameServiceImpl {
public:
	GameServiceImpl() : server_() {
		server_.Post(R"(/start_scenario/)", [&](const httplib::Request& req, httplib::Response& res) {
			auto body_json = json::parse(req.body, nullptr, false);

			if (body_json.is_discarded()) {
				res.status = 400; // Bad Request
				res.set_content("Invalid JSON format", "text/plain");
				return;
			}

			if (!body_json.empty() && body_json["methods"][0].contains("function_name") && body_json["methods"][0].contains("parameters")) {
				std::string function_name = body_json["methods"][0]["function_name"];

				if (function_name == "set_scene") {
					std::string arg;
					if (body_json["methods"][0]["parameters"].contains("args") && !body_json["methods"][0]["parameters"]["args"].empty()) {
						arg = body_json["methods"][0]["parameters"]["args"][0];
					} else {
						res.status = 400; // Bad Request
						res.set_content("Missing 'args' parameter", "text/plain");
						return;
					}

					String scenario_name = String(arg.c_str());
					int fps_limit = int(body_json["fps_limit"]);
					bool headless = !bool(body_json["render"]);
					OS::ProcessID pid = 0;
					ScenarioRunner::get_instance()->run(scenario_name, fps_limit, headless, &pid);
					uint16_t port = ScenarioRunner::get_instance()->get_port(pid);

					res.set_content(std::to_string(port), "text/plain");
				} else {
					res.status = 400; // Bad Request
					res.set_content("Invalid function_name", "text/plain");
				}
			} else {
				res.status = 400; // Bad Request
				res.set_content("Invalid JSON format", "text/plain");
			}
		});


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

							std::string scenario_name_str = scenario["name"].get<std::string>(); // Convertir en std::string
							response_json[scenario_name_str] = port;
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


		server_.new_task_queue = [] { return new httplib::ThreadPool(8); }; // 8 threads
	}

	void run(int port) {
		server_.listen("0.0.0.0", port);
		print_line("HTTP Server started on ", port);
	}

	void close() {
		server_.stop();
	}

private:
	httplib::Server server_;
};


#endif // GODOT_XUMES_COMMUNICATION_H

//
// Created by vincent on 08/05/24.
//

#ifndef GODOT_XUMES_COMMUNICATION_H
#define GODOT_XUMES_COMMUNICATION_H


#include <iostream>
#include <memory>
#include <string>

#include "scene/main/node.h"

#include "core/object/object.h"
#include "scenario_runner.h"

#define CPPHTTPLIB_NO_EXCEPTIONS

#include <iostream>
#include "httplib.h"
#include <cstdlib> // Pour utiliser la fonction std::rand() pour générer un port aléatoire

class GameServiceImpl {
public:
	GameServiceImpl() : server_() {
		server_.Get(R"(/start_scenario/(\w+))", [&](const httplib::Request& req, httplib::Response& res) {
			const char*  name = req.matches[1].str().c_str();

			String scenario_name = String(name);

			OS::ProcessID pid = 0;
			ScenarioRunner::get_instance()->run(scenario_name, &pid);

			uint16_t port = ScenarioRunner::get_instance()->get_port(pid);

			res.set_content(std::to_string(port), "text/plain");
		});
	}

	void run(int port) {
		server_.listen("localhost", port);
		print_line("HTTP Server started on ", port);
	}

private:
	httplib::Server server_;
};



#endif //GODOT_XUMES_COMMUNICATION_H

//
// Created by vincent on 10/02/24.
//

#include <iostream>
#include <fstream>
#include <string>

#include "feature_node.h"
#include "game_state_builder_interface.h"

void FeatureNode::_notification(int p_notification) {
	switch(p_notification) {
		case NOTIFICATION_PROCESS: {
			process();
		} break;
		case NOTIFICATION_READY: {
			set_process(true);
			if (get_tree()->get_current_scene() == this) {
				uint16_t port = ScenarioRunner::get_instance()->get_port();
				server_connection.init_socket(port);
			}
		} break;
		case NOTIFICATION_PREDELETE: {
		} break;
	}
}

void FeatureNode::process() {
	if (get_tree()->get_current_scene() == this) {
		if (!server_connection.is_connected()) {
			server_connection.wait_connection();
		}

		handle_get();
		push_game_state();

	} else {
		if (server_connection.is_connected()) {
			server_connection.stop_socket();
		}
	}
}

void FeatureNode::push_game_state() {
	// get the game actual state
	Dictionary state;
	game_state_builder->get_game_state(this, &state);
	server_connection.post(state);
}

void FeatureNode::handle_get() {

	bool r = true;
	Dictionary event = server_connection.get(r);

	if (r) {
		// handle inputs
		if (event.has("inputs")) {
			if (event["inputs"].is_array()) {
				Array inputs = static_cast<Array>(event["inputs"]);
				input_handler.handle(get_viewport(), inputs);
			}
		}

		// handle notification
		if (event.has("notification")) {
			if (event["event"].is_string()) {
				String notification = static_cast<String>(event["notification"]);
				handle_notification(notification);
			}
		}

		// handle args
		if (event.has("args")) {
			Dictionary args = event["args"];
			if (args.has("given")) {
				given_args = args["given"];
			}

			if (args.has("when")) {
				when_args = args["when"];
			}

			if (args.has("then")) {
				then_args = args["then"];
			}
		}

	}
}


void FeatureNode::handle_notification(String &notification) {

	if (notification == FeatureNode::RESET) {
		reset();
	} else if (notification == FeatureNode::STOP) {
		// do stop
	} else if (notification == FeatureNode::START){
		// do start
	}

}

void FeatureNode::reset() {
	if (GDVIRTUAL_IS_OVERRIDDEN(_when)) {
		GDVIRTUAL_CALL(_when, when_args);
	}
}

void FeatureNode::set_testing_given_str(const String &str) {
	given_str = str;
}

String FeatureNode::get_testing_given_str() const {
	return given_str;
}

void FeatureNode::set_testing_when_str(const String &str) {
	when_str = str;
}

String FeatureNode::get_testing_when_str() const {
	return when_str;
}

void FeatureNode::set_testing_then_str(const String &str) {
	then_str = str;
}

String FeatureNode::get_testing_then_str() const {
	return then_str;
}


FeatureNode::FeatureNode() : game_state_builder(new GameStateBuilderDefault()), server_connection(ServerConnection()), input_handler(InputHandler()){
}

FeatureNode::~FeatureNode() {
	delete game_state_builder;
}

void FeatureNode::_bind_methods() {
	GDVIRTUAL_BIND(_given, "args");
	GDVIRTUAL_BIND(_when, "args");
	GDVIRTUAL_BIND(_then, "args");

	ClassDB::bind_method(D_METHOD("set_testing_given", "str"), &FeatureNode::set_testing_given_str);
	ClassDB::bind_method(D_METHOD("get_testing_given"), &FeatureNode::get_testing_given_str);

	ClassDB::bind_method(D_METHOD("set_testing_when", "str"), &FeatureNode::set_testing_when_str);
	ClassDB::bind_method(D_METHOD("get_testing_when"), &FeatureNode::get_testing_when_str);

	ClassDB::bind_method(D_METHOD("set_testing_then", "str"), &FeatureNode::set_testing_then_str);
	ClassDB::bind_method(D_METHOD("get_testing_then"), &FeatureNode::get_testing_then_str);

	ADD_GROUP("Testing", "testing_");
	ADD_PROPERTY(PropertyInfo(Variant::STRING, "testing_given", PROPERTY_HINT_MULTILINE_TEXT), "set_testing_given", "get_testing_given");
	ADD_PROPERTY(PropertyInfo(Variant::STRING, "testing_when", PROPERTY_HINT_MULTILINE_TEXT), "set_testing_when", "get_testing_when");
	ADD_PROPERTY(PropertyInfo(Variant::STRING, "testing_then", PROPERTY_HINT_MULTILINE_TEXT), "set_testing_then", "get_testing_then");

}

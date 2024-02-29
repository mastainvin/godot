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
				Engine::get_singleton()->set_time_scale(speed);
				DisplayServer::get_singleton()->window_set_vsync_mode(DisplayServer::VSYNC_DISABLED);
				pause_children();
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
	server_connection.post_dict(state);
}

void FeatureNode::handle_get() {

	bool r = true;
	Dictionary event = server_connection.get(r);

	if (r) {
		// handle notification
		if (event.has("event")) {
			if (event["event"].is_string()) {
				String notification = static_cast<String>(event["event"]);
				handle_notification(notification, event);
			}
		}
	}
}


void FeatureNode::handle_notification(String &notification, const Dictionary &event) {

	if (notification == FeatureNode::ACTION) {
		handle_actions(event);
	} else if (notification == FeatureNode::ARGS) {
		handle_args(event);
	} else if (notification == FeatureNode::GIVEN) {
		given();
	} else if (notification == FeatureNode::WHEN) {
		when();
	} else if (notification == FeatureNode::THEN) {
		then();
	} else if (notification == FeatureNode::STOP) {
		server_connection.post_int(1);
		exit(EXIT_SUCCESS);
	} else if (notification == FeatureNode::RUN){
		server_connection.post_int(1);
	} else if (notification == FeatureNode::GET_STATE) {
		push_game_state();
	}

}

void FeatureNode::handle_actions(const Dictionary &event) {
	if (event.has("inputs")) {
		if (event["inputs"].is_array()) {
			inputs = static_cast<Array>(event["inputs"]);
			unpause_children();
			input_handler.handle(get_viewport(), inputs);
			push_game_state();
		}
	} else {
		pause_children();
	}
}

void FeatureNode::handle_args(const Dictionary &event) {
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

		server_connection.post_int(1);
	} else {
		server_connection.post_int(0);
	}
}


void FeatureNode::push_steps() {
	Dictionary steps;
	steps["given"] = given_str;
	steps["when"] = when_str;
	steps["then"] = then_str;

	server_connection.post_dict(steps);
}

void FeatureNode::pause_children() {
	for (int i = 0; i < get_child_count(); i++){
		Node* child = get_child(i);
		child->set_process_mode(Node::PROCESS_MODE_DISABLED);
	}
}


void FeatureNode::unpause_children() {
	for (int i = 0; i < get_child_count(); i++){
		Node* child = get_child(i);
		child->set_process_mode(Node::PROCESS_MODE_INHERIT);
	}
}



void FeatureNode::given() {
	if (GDVIRTUAL_IS_OVERRIDDEN(_given)) {
		GDVIRTUAL_CALL(_given, given_args);
		server_connection.post_int(1);
	}
}

void FeatureNode::when() {
	if (GDVIRTUAL_IS_OVERRIDDEN(_when)) {
		GDVIRTUAL_CALL(_when, when_args);
		server_connection.post_int(1);
	}
}

void FeatureNode::then() {
	if (GDVIRTUAL_IS_OVERRIDDEN(_then)) {
		GDVIRTUAL_CALL(_then, then_args);

		server_connection.post_variant(assertions);
		assertions.clear();
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

void FeatureNode::set_testing_speed(const double s) {
	speed = s;
}

double FeatureNode::get_testing_speed() const {
	return speed;
}


void FeatureNode::assert_equal(const Variant &actual, const Variant &expected) {
	AssertEqual assertion(actual, expected);
	assertions.push_back(assertion.get_dictionary());
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

	ClassDB::bind_method(D_METHOD("set_testing_speed", "s"), &FeatureNode::set_testing_speed);
	ClassDB::bind_method(D_METHOD("get_testing_speed"), &FeatureNode::get_testing_speed);

	ClassDB::bind_method(D_METHOD("assert_equal", "actual", "expected"), &FeatureNode::assert_equal);

	ADD_GROUP("Testing", "testing_");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "testing_speed", PROPERTY_HINT_RANGE, "0,1000,1"), "set_testing_speed", "get_testing_speed");
	ADD_PROPERTY(PropertyInfo(Variant::STRING, "testing_given", PROPERTY_HINT_MULTILINE_TEXT), "set_testing_given", "get_testing_given");
	ADD_PROPERTY(PropertyInfo(Variant::STRING, "testing_when", PROPERTY_HINT_MULTILINE_TEXT), "set_testing_when", "get_testing_when");
	ADD_PROPERTY(PropertyInfo(Variant::STRING, "testing_then", PROPERTY_HINT_MULTILINE_TEXT), "set_testing_then", "get_testing_then");

}

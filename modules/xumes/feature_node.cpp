//
// Created by vincent on 10/02/24.
//

#include <iostream>
#include <fstream>
#include <string>

#include "feature_node.h"
#include "game_state_builder_interface.h"
#include "editor/run_instances_dialog.h"


void FeatureNode::_notification(int p_notification) {

	switch(p_notification) {
		case NOTIFICATION_PROCESS: {
			process();
		} break;
		case NOTIFICATION_READY: {
			set_process(true);
			if (get_tree()->get_current_scene() == this) {
				OS::ProcessID pid = OS::get_singleton()->get_process_id();
				uint16_t port = ScenarioRunner::get_instance()->get_port(pid);
				server_connection->init_socket(port);
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
		if (!server_connection->is_connected() && !already_connected) {
			server_connection->wait_connection();
			already_connected = true;
		} else if(!server_connection->is_connected() && already_connected) {
			exit(EXIT_FAILURE);
		}

		if (!server_connection->is_connected()) {
			exit(EXIT_FAILURE);
		}

		handle_get();
	} else {
		if (server_connection->is_connected()) {
			server_connection->stop_socket();
		}
	}
}

void FeatureNode::push_game_state() {
	// get the game actual state
	Dictionary state;
	game_state_builder->get_game_state(this, &state);
	server_connection->post_dict(state);
}

void FeatureNode::handle_get() {

	bool r = true;
	Dictionary event = server_connection->get(r);
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
		handle_methods(event);
		handle_actions(event);
	} else if (notification == FeatureNode::STOP) {
		server_connection->post_int(1);
		get_tree()->quit(EXIT_SUCCESS);
	} else if (notification == FeatureNode::RUN) {
		server_connection->post_int(1);
	} else if (notification == FeatureNode::RESET) {
		reset();

		// handle_methods(event);
		server_connection->post_int(1);
	} else if (notification == FeatureNode::GET_STATE) {
		push_game_state();
	}



}

void FeatureNode::reset() {
	SceneTree::get_singleton()->reload_current_scene();
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

Error FeatureNode::handle_methods(const Dictionary &event) {
	if (event.has("methods")) {
		if (event["methods"].is_array()) {
			Array methods = static_cast<Array>(event["methods"]);
			for (int i = 0; i < methods.size(); i++) {
				Dictionary method = methods.get(i);

				if (!method.has("function_name") || !method.has("parameters")) {
					return Error::FAILED;
				}

				String method_name = method["function_name"];

				Dictionary parameters = method["parameters"];

				if (!parameters.has("args")) {
					return Error::FAILED;
				}

				Array args = parameters["args"];

				callv(method_name, args);
			}
		}
	}
	return Error::OK;
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



void FeatureNode::set_testing_speed(const double s) {
	speed = s;
}

double FeatureNode::get_testing_speed() const {
	return speed;
}


FeatureNode::FeatureNode() : game_state_builder(new GameStateBuilderDefault()), input_handler(InputHandler()) {
	if (ServerConnection::is_saved()) {
		server_connection = ServerConnection::load();
	} else {
		server_connection = new ServerConnection();
		ServerConnection::save(server_connection);
	}
}

FeatureNode::~FeatureNode() {
	delete game_state_builder;
}

void FeatureNode::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_testing_speed", "s"), &FeatureNode::set_testing_speed);
	ClassDB::bind_method(D_METHOD("get_testing_speed"), &FeatureNode::get_testing_speed);

	ADD_GROUP("Testing", "testing_");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "testing_speed", PROPERTY_HINT_RANGE, "0,1000,1"), "set_testing_speed", "get_testing_speed");
}

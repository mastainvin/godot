//
// Created by vincent on 10/02/24.
//

#ifndef GODOT_FEATURE_NODE_H
#define GODOT_FEATURE_NODE_H

#include "scene/main/node.h"

#include "event_factory.h"
#include "game_state_builder_interface.h"
#include "input_handler.h"
#include "scenario_runner.h"
#include "server_connection.h"

class GameStateBuilderInterface;
class TestHandler;


class FeatureNode : public Node {
	GDCLASS(FeatureNode, Node);

	GDVIRTUAL1(_given, Dictionary);
	GDVIRTUAL1(_when, Dictionary);
	GDVIRTUAL1(_then, Dictionary);

	GameStateBuilderInterface* game_state_builder = nullptr;
	ServerConnection server_connection;
	InputHandler input_handler;

private:
	double time_passed;

	String given_str;
	String when_str;
	String then_str;

	Dictionary given_args;
	Dictionary when_args;
	Dictionary then_args;

	const String RESET = "reset";
	const String START = "start";
	const String STOP = "stop";

	int64_t pid;

protected:
	static void _bind_methods();
	void process();
	void push_game_state();
	void handle_notification(String &notification);
	void handle_get();
	void handle_get_args();



public:
	void _notification(int p_notification);

	void reset();


	void set_testing_given_str(const String &str);
	String get_testing_given_str() const;

	void set_testing_when_str(const String &str);
	String get_testing_when_str() const;

	void set_testing_then_str(const String &str);
	String get_testing_then_str() const;

	FeatureNode();
	~FeatureNode();
};

#endif //GODOT_FEATURE_NODE_H

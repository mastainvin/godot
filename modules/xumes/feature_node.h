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

	GameStateBuilderInterface* game_state_builder = nullptr;
	ServerConnection* server_connection = nullptr;
	InputHandler input_handler;

private:
	double speed = 1.0;

	Array inputs;
	Array assertions;

	const String RUN = "run";
	const String STOP = "finished";
	const String GET_STATE = "get_state";
	const String GET_STEPS = "get_steps";
	const String ACTION = "action";
	const String ARGS = "args";
	const String RESET = "reset";

protected:
	static void _bind_methods();
	void process();
	void push_game_state();
	void handle_notification(String &notification, const Dictionary &event);
	void handle_get();
	void handle_get_args();



public:
	void _notification(int p_notification);

	void handle_actions(const Dictionary &event);
	void handle_args(const Dictionary &event);

	void pause_children();
	void unpause_children();

	void set_testing_given_str(const String &str);
	String get_testing_given_str() const;

	void set_testing_when_str(const String &str);
	String get_testing_when_str() const;

	void set_testing_then_str(const String &str);
	String get_testing_then_str() const;

	void set_testing_speed(const double s);
	double get_testing_speed() const;

	void reset();

	FeatureNode();
	~FeatureNode();
};

#endif //GODOT_FEATURE_NODE_H

//
// Created by vincent on 11/02/24.
//

#ifndef GODOT_GAME_STATE_BUILDER_INTERFACE_H
#define GODOT_GAME_STATE_BUILDER_INTERFACE_H


#include "scene/main/node.h"

class Node;

class GameStateBuilderInterface {

public:
	virtual bool get_game_state(const Node *p_node, Dictionary *p_state) = 0;
	virtual ~GameStateBuilderInterface() = default;
};

class GameStateBuilderDefault : public GameStateBuilderInterface {

public:
	bool get_game_state(const Node *p_node, Dictionary *p_state) override;
};


#endif //GODOT_GAME_STATE_BUILDER_INTERFACE_H

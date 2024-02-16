//
// Created by vincent on 15/02/24.
//

#ifndef GODOT_INPUT_DATA_H
#define GODOT_INPUT_DATA_H

#include <vector>
#include <unordered_set>

#include "core/input/input_event.h"
#include "core/variant/variant.h"

enum ActionType {
	DO = 1,
	UNDO = 2
};

enum EventType {
	ACTION_EVENT = 1,
	JOY_MOTION_EVENT = 2,
	JOY_BUTTON_EVENT = 3,
	NO_EVENT = 4,
};

struct InputData {

	EventType event_type;
	String action_name;
	ActionType action;
	JoyButton button;
	JoyAxis axis;
	float  axis_value = 0.0f;

	bool operator==(const InputData &other) const {
		bool b_event_type = other.event_type == event_type;
		bool b_action_name = other.action_name == action_name;
		bool b_action = other.action == action;
		bool b_button = other.button == button;
		bool b_axis = other.axis == axis;
		bool b_axis_value = other.axis_value == axis_value;

		return b_event_type && b_action_name && b_action && b_button && b_axis && b_axis_value;
	};

	struct HashFunction
	{
		size_t operator()(const InputData& d) const
		{
			size_t event_type_hash = std::hash<int>()(d.event_type);
			size_t action_name_hash = d.action_name.hash();
			return event_type_hash ^ action_name_hash;
		}
	};


};




#endif //GODOT_INPUT_DATA_H

//
// Created by vincent on 15/02/24.
//

#include "translator.h"

Translator* Translator::singleton = nullptr;;

Translator *Translator::get_instance() {
	if (singleton == nullptr) {
		singleton = new Translator();
	}
	return singleton;
}
Translator::Translator() {}


InputData Translator::translate_input(Dictionary input) {

	InputData input_data = InputData();
	if (input.has("type")) {
		bool r_found;
		String type = input["type"];
		input_data.event_type = translate_event_type(type, r_found);
	}

	if (input.has("action_name")) {
		String action_name = input["action_name"];
		input_data.action_name = action_name;
	}
	if (input.has("button")) {
		String button = input["button"];
		input_data.button = translate_joy_button(button);
	}

	if (input.has("axis")) {
		String axis = input["axis"];
		input_data.axis = translate_joy_axis(axis);
	}

	if (input.has("axis_value")) {
		float axis_value = input["axis_value"];
		input_data.axis_value = axis_value;
	}

	input_data.action = ActionType::DO;

	return input_data;
}




JoyButton Translator::translate_joy_button(String &button_name) {
	if (button_name == "A") {
		return JoyButton::A;
	} else if (button_name == "X"){
		return JoyButton::X;
	} else if (button_name == "B") {
		return JoyButton::B;
	} else if (button_name == "Y") {
		return JoyButton::Y;
	}

	return JoyButton::A;
}

JoyAxis Translator::translate_joy_axis(String &axis) {
	if (axis == "LEFT_X") {
		return JoyAxis::LEFT_X;
	} else if (axis == "LEFT_Y") {
		return JoyAxis::LEFT_Y;
	} else if (axis == "RIGHT_X") {
		return JoyAxis::RIGHT_X;
	} else if (axis == "RIGHT_Y") {
		return JoyAxis::RIGHT_Y;
	}

	return JoyAxis::LEFT_X;
}

EventType Translator::translate_event_type(String &event_type_str, bool &r_found) {

	r_found = true;
	if (event_type_str == "JOY_BUTTON_EVENT") {
		return EventType::JOY_BUTTON_EVENT;
	} else if (event_type_str == "ACTION_EVENT") {
		return EventType::ACTION_EVENT;
	} else if (event_type_str == "JOY_MOTION_EVENT"){
		return EventType::JOY_MOTION_EVENT;
	} else {
		r_found = false;
		return EventType::NO_EVENT;
	}
}

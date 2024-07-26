//
// Created by vincent on 14/02/24.
//

#include "input_handler.h"

void InputHandler::handle(Viewport *viewport, Array &inputs) {
	Dictionary input;
	std::unordered_set<InputData, InputData::HashFunction> input_set;
	Ref<InputEvent> ref_event;
	InputData input_data;

	for (int i = 0; i < inputs.size() ; i++) {
		input = inputs[i];

		input_data = Translator::get_instance()->translate_input(input);
		input_set.insert(input_data);
		ref_event = EventFactory::get_instance()->instantiate_event(input_data);
		Input::get_singleton()->action_press(input_data.action_name);
		viewport->push_input(ref_event);
	}

	for (InputData input_data_pressed : pressed_inputs) {

		if (input_set.find(input_data_pressed) == input_set.end()) {
			input_data_pressed.action = ActionType::UNDO;
			ref_event = EventFactory::get_instance()->instantiate_event(input_data_pressed);
			Input::get_singleton()->action_release(input_data_pressed.action_name);
			viewport->push_input(ref_event);
		}
	}

	pressed_inputs = input_set;
}

void InputHandler::flush(Viewport *viewport) {
	Ref<InputEvent> ref_event;
	
	for (InputData input_data_pressed : pressed_inputs) {
			input_data_pressed.action = ActionType::UNDO;
			ref_event = EventFactory::get_instance()->instantiate_event(input_data_pressed);
			Input::get_singleton()->action_release(input_data_pressed.action_name);
			viewport->push_input(ref_event);
	}
	pressed_inputs.clear();
}
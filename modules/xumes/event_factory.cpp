//
// Created by vincent on 13/02/24.
//


#include "event_factory.h"

Ref<InputEvent> EventFactory::instantiate_event(InputData &input_data) {

	switch (input_data.event_type) {
		case EventType::ACTION_EVENT: {
			return create_action_event(input_data);
		} break;
		case EventType::JOY_MOTION_EVENT: {
			return create_joy_button_event(input_data);
		} break;
		case EventType::JOY_BUTTON_EVENT: {
			return create_joy_button_event(input_data);
		} break;
		default: {

		}break;
	}

	return Ref<InputEvent>();
}

Ref<InputEvent> EventFactory::create_action_event(InputData &input_data) {
	Ref<InputEventAction> refEventAction;
	refEventAction.instantiate();
	refEventAction->set_action(input_data.action_name);
	refEventAction->set_pressed(input_data.action == ActionType::DO);

	return static_cast<Ref<InputEvent>>(refEventAction);
}


Ref<InputEvent> EventFactory::create_joy_button_event(InputData &input_data) {
	Ref<InputEventJoypadButton> refJoyButton;
	refJoyButton.instantiate();
	refJoyButton->set_button_index(input_data.button);
	refJoyButton->set_pressed(input_data.action == ActionType::DO);

	return (static_cast<Ref<InputEvent>>(refJoyButton));
}


Ref<InputEvent> EventFactory::create_joy_motion_event(InputData &input_data) {
	Ref<InputEventJoypadMotion> refJoyMotion;
	refJoyMotion.instantiate();
	refJoyMotion->set_axis(input_data.axis);
	refJoyMotion->set_axis_value(input_data.axis_value);

	return static_cast<Ref<InputEvent>>(refJoyMotion);
}


EventFactory* EventFactory::singleton = nullptr;;

EventFactory *EventFactory::get_instance() {
	if (singleton == nullptr) {
		singleton = new EventFactory();
	}

	return singleton;

}

EventFactory::EventFactory() {}
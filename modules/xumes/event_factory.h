//
// Created by vincent on 13/02/24.
//

#ifndef GODOT_EVENT_FACTORY_INTERFACE_H
#define GODOT_EVENT_FACTORY_INTERFACE_H

#include "core/input/input_event.h"

#include "input_data.h"

class EventFactory {

private:
	static EventFactory *singleton;

	Ref<InputEvent> create_action_event(InputData &input_data) ;
	Ref<InputEvent> create_joy_button_event(InputData &input_data);
	Ref<InputEvent> create_joy_motion_event(InputData &input_data);


public:
	Ref<InputEvent> instantiate_event(InputData &input_data);


	static EventFactory *get_instance();
	EventFactory(EventFactory &other) = delete;
	void operator=(const EventFactory &) = delete;

	EventFactory();
};

#endif //GODOT_EVENT_FACTORY_INTERFACE_H

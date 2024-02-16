//
// Created by vincent on 15/02/24.
//

#ifndef GODOT_TRANSLATOR_H
#define GODOT_TRANSLATOR_H

#include "core/variant/array.h"
#include "core/input/input_event.h"

#include "input_data.h"

class Translator {


private:
	static Translator* singleton;

	JoyButton translate_joy_button(String &button_name);
	JoyAxis translate_joy_axis(String &axis);
	EventType translate_event_type(String &event_type_str, bool &r_found);

public:


	InputData translate_input(Dictionary input);

	static Translator *get_instance();
	Translator(Translator &other) = delete;
	void operator=(const  Translator &) = delete;


	Translator();
};

#endif //GODOT_TRANSLATOR_H

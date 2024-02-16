//
// Created by vincent on 14/02/24.
//

#ifndef GODOT_TESTER_HANDLER_H
#define GODOT_TESTER_HANDLER_H

#include <unordered_set>

#include "scene/main/node.h"
#include "scene/main/viewport.h"
#include "core/variant/array.h"
#include "core/input/input_event.h"

#include "translator.h"
#include "event_factory.h"
#include "input_data.h"



class InputHandler {


private:
	std::unordered_set<InputData, InputData::HashFunction> pressed_inputs;

public:
	void handle(Viewport *viewport, Array &inputs);


};

#endif //GODOT_TESTER_HANDLER_H

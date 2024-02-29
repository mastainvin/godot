//
// Created by vincent on 28/02/24.
//

#ifndef GODOT_ASSERTION_H
#define GODOT_ASSERTION_H

#include "scene/main/node.h"

class Assertion {


public:
	Dictionary get_dictionary();
};



class AssertEqual : public Assertion {

private:
	Variant expected;
	Variant actual;
public:
	Dictionary get_dictionary();
	AssertEqual(const Variant &actual, const Variant &expected);
};


#endif //GODOT_ASSERTION_H

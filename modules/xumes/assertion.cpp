//
// Created by vincent on 28/02/24.
//

#include "assertion.h"


Dictionary AssertEqual::get_dictionary() {

	Dictionary dict;

	dict["type"] = "assert_equal";
	dict["expected"] = expected;
	dict["actual"] = actual;

	return  dict;
}

AssertEqual::AssertEqual(const Variant &actual, const Variant &expected) : actual(actual), expected(expected) {

}
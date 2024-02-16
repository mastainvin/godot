//
// Created by vincent on 10/02/24.
//

#include "register_types.h"

#include "core/config/engine.h"
#include "core/object/class_db.h"

#include "feature_node.h"
#include "xumes_communication.h"
#include "scenario_runner.h"

static XumesCommunication *xumes_communication = nullptr;
static ScenarioRunner *scenario_runner = nullptr;

void initialize_xumes_module(ModuleInitializationLevel p_level) {


	if (p_level == MODULE_INITIALIZATION_LEVEL_SCENE) {
		ClassDB::register_class<FeatureNode>();

		xumes_communication = memnew(XumesCommunication);
		xumes_communication->init();
	}




	if (p_level == MODULE_INITIALIZATION_LEVEL_CORE) {
		scenario_runner = memnew(ScenarioRunner);
	}

}

void uninitialize_xumes_module(ModuleInitializationLevel p_level) {

	if (p_level == MODULE_INITIALIZATION_LEVEL_SCENE) {
		if (xumes_communication) {
			xumes_communication->finish();
			memdelete(xumes_communication);
		}
	}


	if (p_level == MODULE_INITIALIZATION_LEVEL_CORE) {
		if (scenario_runner) {
			memdelete(scenario_runner);
		}
	}


}





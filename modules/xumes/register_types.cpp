//
// Created by vincent on 10/02/24.
//

#include "register_types.h"

#include "core/object/class_db.h"

#include "feature_node.h"
#include "scenario_runner.h"
#include "xumes_communication_thread.h"

static XumesCommunicationThread *xumes_communication_thread = NULL;
static ScenarioRunner *scenario_runner = NULL;


void initialize_xumes_module(ModuleInitializationLevel p_level) {


	if (p_level == MODULE_INITIALIZATION_LEVEL_SCENE) {
		ClassDB::register_class<FeatureNode>();


	}


	if (p_level == MODULE_INITIALIZATION_LEVEL_EDITOR) {
		if (!xumes_communication_thread) {
			xumes_communication_thread = memnew(XumesCommunicationThread);
			xumes_communication_thread->init();
		}

		if (!scenario_runner) {
			scenario_runner = memnew(ScenarioRunner);
		}

	}



}

void uninitialize_xumes_module(ModuleInitializationLevel p_level) {

	if (p_level == MODULE_INITIALIZATION_LEVEL_SCENE) {

	}
	if (p_level == MODULE_INITIALIZATION_LEVEL_EDITOR) {
		if (xumes_communication_thread) {
			xumes_communication_thread->finish();
			memdelete(xumes_communication_thread);
		}
	}

	if (p_level == MODULE_INITIALIZATION_LEVEL_CORE) {


		if (scenario_runner) {
			memdelete(scenario_runner);
		}
	}


}

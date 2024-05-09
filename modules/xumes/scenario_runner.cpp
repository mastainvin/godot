//
// Created by vincent on 12/02/24.
//

#include "scenario_runner.h"

#include "core/config/project_settings.h"
#include "editor/debugger/editor_debugger_node.h"
#include "editor/editor_node.h"
#include "editor/editor_settings.h"
#include "editor/run_instances_dialog.h"
#include "main/main.h"
#include "servers/display_server.h"




ScenarioRunner::Status ScenarioRunner::get_status() const {
	return status;
}


int64_t ScenarioRunner::run(const String &p_scene, OS::ProcessID  *r_pid) {

	List<String> args;

	// args.push_back("--headless");

	if (!p_scene.is_empty()) {
		args.push_back("res://Tests/PipeSizeTest/PipeSizeTest.tscn");
	}

	List<String> instance_args(args);
	RunInstancesDialog::get_singleton()->get_argument_list_for_instance(0, instance_args);

	Error err = OS::get_singleton()->create_instance(instance_args, r_pid);
	//Error err = OS::get_singleton()->create_process(OS::get_singleton()->get_executable_path(), args, &pid, true);

	ERR_FAIL_COND_V(err, err);

	if (r_pid != 0) {
		pids.push_back(*r_pid);
	}

	status = STATUS_PLAY;

	return OK;
}


uint16_t ScenarioRunner::get_port(OS::ProcessID &pid){
	return 1024 + pid % (65535 - 1024 + 1);
}



void ScenarioRunner::stop() {
	if (status != STATUS_STOP && pids.size() > 0) {
		for (const OS::ProcessID &E : pids) {
			OS::get_singleton()->kill(E);
		}
		pids.clear();
	}

	status = STATUS_STOP;
}



ScenarioRunner *ScenarioRunner::singleton = nullptr;

ScenarioRunner *ScenarioRunner::get_instance() {
	return singleton;
}


ScenarioRunner::ScenarioRunner() {
	status = STATUS_STOP;
	singleton = this;
}
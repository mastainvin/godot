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


int64_t ScenarioRunner::run(const String &p_scene) {
	List<String> args;

	//args.push_back("--headless");

	args.push_back(p_scene);

	String exec = OS::get_singleton()->get_executable_path();
	List<String> instance_args(args);
	RunInstancesDialog::get_singleton()->get_argument_list_for_instance(0, instance_args);

	if (OS::get_singleton()->is_stdout_verbose()) {
		print_line(vformat("Running: %s", exec));
		for (const String &E : instance_args) {
			print_line(" %s", E);
		}
	}

	int64_t pid = 0;
	Error err = OS::get_singleton()->create_process(OS::get_singleton()->get_executable_path(), instance_args, &pid, true);
	ERR_FAIL_COND_V(err, err);
	if (pid != 0) {
		pids.push_back(pid);
	}

	status = STATUS_PLAY;

	return OK;
}


uint16_t ScenarioRunner::get_port(){
	previous_port++;
	return previous_port;
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
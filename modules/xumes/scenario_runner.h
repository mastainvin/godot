//
// Created by vincent on 12/02/24.
//

#ifndef GODOT_SCENARIO_RUNNER_H
#define GODOT_SCENARIO_RUNNER_H

#include <map>
#include <unistd.h>
#include <iostream>

#include "core/os/os.h"

class ScenarioRunner: public Object {
	GDCLASS(ScenarioRunner, Object);


public:
	enum Status {
		STATUS_PLAY,
		STATUS_PAUSED,
		STATUS_STOP
	};

	List<OS::ProcessID> pids;


private:
	Status status;

	static ScenarioRunner *singleton;

public:
	Status get_status() const;

	int64_t run(const String &p_scene, int fps_limit, bool headless, OS::ProcessID  *r_pid);
	void stop();

	uint16_t get_port(OS::ProcessID &pid);

	static ScenarioRunner* get_instance();

	ScenarioRunner();

};

#endif //GODOT_SCENARIO_RUNNER_H

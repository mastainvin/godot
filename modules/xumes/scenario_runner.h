#ifndef GODOT_SCENARIO_RUNNER_H
#define GODOT_SCENARIO_RUNNER_H

#include <map>
#include <unistd.h>
#include <iostream>
#include "core/os/os.h"

class ScenarioRunner : public Object {
	GDCLASS(ScenarioRunner, Object);

public:
	enum Status {
		STATUS_PLAY,
		STATUS_PAUSED,
		STATUS_STOP
	};

	Dictionary ports_pids;

private:
	Status status;

	static ScenarioRunner* singleton;

public:
	static ScenarioRunner* get_instance();

	Status get_status() const;
	int64_t run(const String& p_scene, int fps_limit, bool headless, OS::ProcessID* r_pid);
	void stop(OS::ProcessID pid);
	uint16_t get_port(OS::ProcessID& pid);

private:
	ScenarioRunner();
	~ScenarioRunner();
};

#endif // GODOT_SCENARIO_RUNNER_H

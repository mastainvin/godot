//
// Created by vincent on 12/02/24.
//

#ifndef GODOT_SCENARIO_RUNNER_H
#define GODOT_SCENARIO_RUNNER_H


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

	uint16_t previous_port = 8080;

private:
	Status status;

	static ScenarioRunner *singleton;

public:
	Status get_status() const;

	int64_t run(const String &p_scene);
	void stop();

	uint16_t get_port();

	static ScenarioRunner* get_instance();

	ScenarioRunner();

};

#endif //GODOT_SCENARIO_RUNNER_H

#pragma once

#include <cstdint>
#include <memory>

#include "Event.h"
#include "SchedTypes.h"

class TimeWheel {
public:
	const int MAX_RES_NODES = 64;
	const int MAX_RES_OFFER = 200;
	const int MAX_JOB_DURATION = 100;
	const int MAX_JOB_REQUIREMENT = 50;
	const int MAX_JOB_INTER_ARRIVAL = 3;
	const int MAX_OFFER_INTER_ARRIVAL = 5;
	TimeWheel(JobScheduler::Policy policy, std::int64_t numCycles, short int verbosity);
	void Simulate(void);

private:
	void ScheduleResOffer(Time_t curTime);
	void ScheduleJobArrival(Time_t curTime);
	void DumpEventQueue(void);

private:

	short int _verbosity;
	Time_t _curTime;
	std::int64_t _maxCycles;
	std::priority_queue<std::shared_ptr<Event>, std::vector<std::shared_ptr<Event>>, EventComp> _eventQueue;	
	JobScheduler _scheduler;
};

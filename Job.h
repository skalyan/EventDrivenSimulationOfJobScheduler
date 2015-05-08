#pragma once

#include <cstdint>
#include <iostream>
#include <memory>

#include "SchedTypes.h"
#include "Node.h"

class Job {
public:
	enum class JobState : std::int8_t {
		Queued,
		Waiting,
		Running,
		Completed	
	};

	static std::shared_ptr<Job> MakeJob(Time_t curTime, Time_t duration, ULong resReqd);

	void Dump(void) {
		std::cout << _id << ":<" << _duration << ", " <<  _requirement << "> "; 
	}

	bool operator < (const std::shared_ptr<Job> other) {
		std::cout << " Comparing jobs..." << std::endl;
		return _requirement > other->_requirement;
	}

	ID_t GetId(void) const {
		return _id;
	}
	
	ULong GetRequirement(void) const {
		return _requirement;
	}

	Time_t GetDuration(void) const {
		return _duration;
	}

	Time_t GetEntryTime(void) const {
		return _entryTime;
	}

	Time_t GetQueuedDuration(void) const {
		return _queuedDuration;
	}

	bool IsRunning(void) const {
		return _state == JobState::Running;
	}

	bool IsCompleted(void) const {
		return _state == JobState::Completed;
	}

	std::shared_ptr<Node> GetRunningOn(void) const {
		return _runningOn;
	}

	bool PlaceOn(Time_t curTime, std::shared_ptr<Node> host) {
		assert(_state == JobState::Queued ||
				 _state == JobState::Waiting);
		if (!host->HasAvailable(_requirement)) {
			return false;
		}
		host->Consume(_requirement);
		_runningOn = host;
		std::cout << "\t\t\t@" << curTime << ">>>>>Job " << _id << " with " << _requirement << " Placed on " << host->GetId() << std::endl;
		LogQueueTime(curTime);
		_state = JobState::Running;	
		return true;
	}

	bool IsPlaceable(std::shared_ptr<Node> host) {
		return host->HasAvailable(_requirement);
	}

	void LogQueueTime(Time_t curTime) {
		assert(curTime >= _entryTime);	
		_queuedDuration = curTime - _entryTime;
	}
	
	
	void Finish(Time_t curTime) {
		assert(_runningOn != nullptr);
		assert(_state == JobState::Running);
		// _runningOn->Release(_requirement);
		_state = JobState::Completed;
		std::cout << "<<<<<<Job " << _requirement << " finished at " << curTime << std::endl;
		_finishTime = curTime;
		_runningOn = nullptr;
	}

	void MarkWaitState(Time_t curTime) {
		_state = JobState::Waiting;
	}
	
private:
	Job(ID_t id, Time_t entryTime, Time_t duration, ULong reqmnt) :
		_id(id), _requirement(reqmnt), _duration(duration),
		_entryTime(entryTime), _queuedDuration(0), _finishTime(0),
		_state(JobState::Queued), _runningOn(nullptr) {
   }

private:
	static ID_t           _nextId;

	ID_t                  _id;
	ULong                 _requirement; 	// Resource requirement.
	Time_t                _duration;    	// Time requirement.
	Time_t                _entryTime;   	// Entry time into the system.
	Time_t                _queuedDuration; // Time spent waiting in the queue.
	Time_t                _finishTime;
	JobState              _state;
	std::shared_ptr<Node> _runningOn;
};

class JobPrioritizer {
public:
	virtual bool operator()( const std::shared_ptr<Job> a, const std::shared_ptr<Job> b)
	{
		std::cout << " PANICCCCCCCCCCC" << std::endl;
		#ifdef DEBUG_LEVEL5
		std::cout << " CompareJobs left " << a->GetRequirement() << " right " << b->GetRequirement() << std::endl;
		#endif
		return a->GetRequirement() < b->GetRequirement();
	}
};

class RawDemandPrioritizer : public JobPrioritizer {
public:
	virtual bool operator()( const std::shared_ptr<Job> a, const std::shared_ptr<Job> b)
	{
		#ifdef DEBUG_LEVEL5
		std::cout << " CompareJobs left " << a->GetRequirement() << " right " << b->GetRequirement() << std::endl;
		#endif
		return a->GetRequirement() < b->GetRequirement();
	}
};

class TimeWeightedDemandPrioritizer : public JobPrioritizer { 
public:
	virtual bool operator()( const std::shared_ptr<Job> a, const std::shared_ptr<Job> b)
	{
		#ifdef DEBUG_LEVEL5
		std::cout << " CompareJobs left " << a->GetRequirement() * a->GetDuration()
					 << " right " << b->GetRequirement() * b->GetDuration() << std::endl;
		#endif
		return a->GetRequirement() * a->GetDuration() > b->GetRequirement() * b->GetDuration();
	}
};

class FCFSPrioritizer : public JobPrioritizer {
public:
	virtual bool operator()( const std::shared_ptr<Job> a, const std::shared_ptr<Job> b)
	{
		#ifdef DEBUG_LEVEL5
		std::cout << " CompareJobs left " << a->GetEntryTime() << " right " << b->GetEntryTime() << std::endl;
		#endif
		return a->GetEntryTime() < b->GetEntryTime();
	}
};

bool JobPrioritizerFn(const std::shared_ptr<Job> a, const std::shared_ptr<Job> b);
bool RawDemandPrioritizerFn(const std::shared_ptr<Job> a, const std::shared_ptr<Job> b);
bool TimeWeightedDemandPrioritizerFn(const std::shared_ptr<Job> a, const std::shared_ptr<Job> b);
bool FCFSPrioritizerFn(const std::shared_ptr<Job> a, const std::shared_ptr<Job> b);

typedef bool (*QuePolicyFn) (const std::shared_ptr<Job>, const std::shared_ptr<Job>);

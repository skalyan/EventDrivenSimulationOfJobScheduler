#pragma once

#include <cstdint>
#include <memory>
#include <queue>
#include <vector>
#include <list>
#include <map>
#include <math.h>


#include "SchedTypes.h"

#include "Job.h"
#include "Node.h"

class Event;

class JobScheduler {
public:
	enum class Policy {
		FCFS,
		TimeWeightedDemand,
		RawDemand
	};

	JobScheduler(Policy policy) : _jobCount(0), _totalWaitDuration(0) {
		switch (policy) {
			case Policy::RawDemand:
				_queuePolicy = RawDemandPrioritizerFn;
				break;
			case Policy::TimeWeightedDemand:
				_queuePolicy = TimeWeightedDemandPrioritizerFn;
				break;
			case Policy::FCFS:
				_queuePolicy = FCFSPrioritizerFn;
				break;
		}
	}
	
	void RunAlgo(Time_t curTime, std::list<std::shared_ptr<Event>>& eventList);

	std::shared_ptr<Event> PlaceJob(Time_t curTime, std::shared_ptr<Job> job);

	void RegisterOffer(const ResOffer_t& offer);

	void CreateJob(Time_t duration, ULong resReqd);

	void QueueJob(Time_t curTime, std::shared_ptr<Job> job);
	void AccountFinishedJob(std::shared_ptr<Job> job);

	void VerifyPlacement(Time_t curTime);
	void PrintFreeCapacity(Time_t curTime);

	int GetJobCount(void) {
		return _jobCount;
	}

	int GetUnplacedJobCount(void) {
		return _jobQueue.size();
	}

	Time_t GetWaitDuration(void) {
		return _totalWaitDuration;
	}

	Time_t GetAvgWaitDuration(void) {
		return (_jobCount > 0)  ? ceil(_totalWaitDuration / _jobCount)
										: 0;
	}

private:
	void DumpNodeCapacities(void);
	void DumpJobQueue(void);

private:
	int _jobCount;
	Time_t _totalWaitDuration;
	std::vector<std::shared_ptr<Job> > _jobQueue;

	std::vector<std::shared_ptr<Node> >        _nodeList;
	std::map<ID_t, std::shared_ptr<Node> >     _nodeTable;

	QuePolicyFn _queuePolicy;	
};

#include <cstdint>
#include <memory>

#include "Job.h"
#include "SchedTypes.h"

ID_t Job::_nextId = 0;

std::shared_ptr<Job>
Job::MakeJob(Time_t curTime, Time_t duration, ULong resReqd)
{
	std::shared_ptr<Job> job(new Job(_nextId++, curTime, duration, resReqd));
	return job;		
}

bool JobPrioritizerFn(const std::shared_ptr<Job> a, const std::shared_ptr<Job> b)
{
	std::cout << " PANICCCCCCCCCCC" << std::endl;
	#ifdef DEBUG_LEVEL5
	std::cout << " CompareJobs left " << a->GetRequirement() << " right " << b->GetRequirement() << std::endl;
	#endif
	return a->GetRequirement() < b->GetRequirement();
}

bool RawDemandPrioritizerFn(const std::shared_ptr<Job> a, const std::shared_ptr<Job> b)
{
	#ifdef DEBUG_LEVEL5
	std::cout << " CompareJobs left " << a->GetRequirement() << " right " << b->GetRequirement() << std::endl;
	#endif
	return a->GetRequirement() < b->GetRequirement();
}

bool TimeWeightedDemandPrioritizerFn(const std::shared_ptr<Job> a, const std::shared_ptr<Job> b)
{
	#ifdef DEBUG_LEVEL5
	std::cout << " CompareJobs left " << a->GetRequirement() * a->GetDuration()
				 << " right " << b->GetRequirement() * b->GetDuration() << std::endl;
	#endif
	return a->GetRequirement() * a->GetDuration() > b->GetRequirement() * b->GetDuration();
}

bool FCFSPrioritizerFn(const std::shared_ptr<Job> a, const std::shared_ptr<Job> b)
{
	#ifdef DEBUG_LEVEL5
	std::cout << " CompareJobs left " << a->GetEntryTime() << " right " << b->GetEntryTime() << std::endl;
	#endif
	return a->GetEntryTime() < b->GetEntryTime();
}

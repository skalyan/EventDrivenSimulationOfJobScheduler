
#include <cstdint>
#include <memory>
#include <iostream>

#include "Event.h"
#include "SchedTypes.h"
#include "TimeWheel.h"

TimeWheel::TimeWheel(JobScheduler::Policy policy, std::int64_t numCycles, short int verbosity) :
	_curTime(0), _maxCycles(numCycles), _scheduler(policy), _verbosity(verbosity)
{
}

void
TimeWheel::DumpEventQueue(void)
{
	if (_verbosity > 3) {
		std::priority_queue<std::shared_ptr<Event>, std::vector<std::shared_ptr<Event>>, EventComp> lq(_eventQueue);	
	
		std::cout << "EventQueue : " << std::endl;
		std::shared_ptr<Event> ev;
		while (!lq.empty()) {
			ev = lq.top();
			ev->Dump();
			lq.pop();
		}
	}
}

/*
 * Simulate --
 *		Runs the simulation for _maxCycles.
 *		Each simulation cycle does the following:
 *		-
 *		- Loop Begin
 *		- - 
 *		- - Inner Loop Begin
 *		- - - 1 Pick earliest event and call the handler.
 *		- - - 2 If the handler needs to schedule another event, it is queued.
 *		- - - 3 If More events at current time, go to 1.
 *		- - Inner Loop End
 *		- - 
 *		- - Run Job Scheduler.
 *		- - Place the jobs 
 *		- - Queue resulting events from the placement.
 *		-
 *		- Loop End
 *		- Print Summary Statistics.
 */
void
TimeWheel::Simulate()
{
	// Bootstrap the simulator by reading in one resource offer and one job.
	ScheduleResOffer(_curTime);
	ScheduleJobArrival(_curTime);

	for (auto cycle = 0; cycle < _maxCycles && !_eventQueue.empty(); ++cycle) {
		do {
			DumpEventQueue();

			std::shared_ptr<Event> curEvent = _eventQueue.top();
			_eventQueue.pop();
			_curTime = curEvent->GetTime();

			if (_verbosity > 0) {
				std::cout << "Time = " << _curTime << std::endl;
			}

			// Handle current event and schedule a new event, if needed.
			std::shared_ptr<Event> newEvt = curEvent->Handle(_curTime, _scheduler);
			if (newEvt != nullptr) {
				#ifdef DEBUG_LEVEL4
				std::cout << "newEvent = " << newEvt->GetTime() << std::endl;
				#endif
				_eventQueue.push(newEvt);
			}

			#ifdef DEBUG_LEVEL4
			std::cout << "eventQue size = " << _eventQueue.size() << std::endl;
			#endif
		} while (!_eventQueue.empty() && _eventQueue.top()->GetTime() == _curTime);

		/*
		 * Invoke JobScheduler and collect new events resulting from placements.
		 * These new events will be processed in the next cycle.
		 */
		std::list<std::shared_ptr<Event>> newEvents;
		_scheduler.RunAlgo(_curTime, newEvents);
		for (auto e : newEvents) {
			#ifdef DEBUG_LEVEL4
			std::cout << "new job Events = " << e->GetTime() << std::endl;
			#endif
			_eventQueue.push(e);
		}

		// Verify that there is no host with enough resources to host a job.
		_scheduler.VerifyPlacement(_curTime);

		if (_verbosity > 2) {
			_scheduler.PrintFreeCapacity(_curTime);
		}
	}

	// Dump summary statistics.
	if (_scheduler.GetUnplacedJobCount() > 0) {
		std::cout << "Number of jobs not placed : " << _scheduler.GetUnplacedJobCount()
				 	 << std::endl;
	}
	std::cout << "Total number of jobs finished : " << _scheduler.GetJobCount()
				 << std::endl;
	std::cout << "Total time spent waiting in the queue by jobs : "
				 << _scheduler.GetWaitDuration() << std::endl;
	std::cout << "Avg. time spent jobs waiting in the queue : "
				 << _scheduler.GetAvgWaitDuration() << std::endl;
}

void
TimeWheel::ScheduleResOffer(Time_t curTime)
{
	_eventQueue.push(Event::MakeResOfferReadEvent(curTime));
}

void
TimeWheel::ScheduleJobArrival(Time_t curTime)
{
	_eventQueue.push(Event::MakeJobArrivalEvent(curTime));
}

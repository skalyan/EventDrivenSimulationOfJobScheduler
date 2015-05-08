#pragma once

#include <cstdint>
#include <memory>

#include "SchedTypes.h"

#include "Job.h"
#include "Node.h"
#include "JobScheduler.h"


class Event {
public:
	static std::shared_ptr<Event> MakeJobArrivalEvent(Time_t eTime);
	static std::shared_ptr<Event> MakeResOfferReadEvent(Time_t eTime);
	static std::shared_ptr<Event> MakeJobCompletedEvent(Time_t eTime, std::shared_ptr<Job> job);

	explicit Event(Time_t etime) : _eTime(etime) { }

	virtual std::shared_ptr<Event> Handle(Time_t, JobScheduler&) = 0;
	virtual void Dump(void) {
		std::cout << " " << _eTime;
	}
	Time_t GetTime(void) const {
		return _eTime;
	}

private:
	Time_t _eTime;
};

/*
 * Processes one job arrival read event in the handler.
 */
class JobArrivalEvent : public Event {
public:
	explicit JobArrivalEvent(Time_t eTime) :
		Event(eTime) { }
	std::shared_ptr<Event> Handle(Time_t curTime, JobScheduler& );
	virtual void Dump(void) {
		std::cout << "\t\t JobArrival ";
		Event::Dump();
		std::cout << std::endl;
	}

private:
	static std::ifstream JobStream;
};

/*
 * Processes one resource offer read event in the handler.
 */
class ResOfferReadEvent : public Event {
public:
	explicit ResOfferReadEvent(Time_t eTime) :
		Event(eTime) { }
	std::shared_ptr<Event> Handle(Time_t curTime, JobScheduler& );
	virtual void Dump(void) {
		std::cout << "\t\t OfferRead ";
		Event::Dump();
		std::cout << std::endl;
	}

private:
	static std::ifstream OfferStream;
};

/*
 * Processes one job complete event in the handler.
 */
class JobCompletedEvent : public Event {
public:
	JobCompletedEvent(Time_t eTime, std::shared_ptr<Job> job) :
		Event(eTime), _job(job) { }

	std::shared_ptr<Event> Handle(Time_t curTime, JobScheduler& );

	virtual void Dump(void) {
		std::cout << "\t\t JobFinish ";
		Event::Dump();
		std::cout << std::endl;
	}

private:
	std::shared_ptr<Job> _job;
};

class EventComp {
public:
	bool operator()(const std::shared_ptr<Event> a, const std::shared_ptr<Event> b)
	{
		return a->GetTime() > b->GetTime();
	}
};

#include <cstdint>
#include <memory>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <random>

#include "Event.h"

extern short int Verbosity;
std::shared_ptr<Event>
Event::MakeJobArrivalEvent(Time_t time)
{
	std::shared_ptr<JobArrivalEvent> ev(new JobArrivalEvent(time));
	return ev;
}

std::shared_ptr<Event>
Event::MakeJobCompletedEvent(Time_t time, std::shared_ptr<Job> job)
{
	std::shared_ptr<JobCompletedEvent> ev(new JobCompletedEvent(time, job));
	return ev;
}

std::shared_ptr<Event>
Event::MakeResOfferReadEvent(Time_t time)
{
	std::shared_ptr<ResOfferReadEvent> ev(new ResOfferReadEvent(time));
	return ev;
}

std::ifstream ResOfferReadEvent::OfferStream("offerStream.txt");
std::ifstream JobArrivalEvent::JobStream("jobStream.txt");

namespace { // anonymous namespace

const int MAX_OFFER_INTER_ARRIVAL = 5;
const int MAX_JOB_INTER_ARRIVAL = 3 ;

std::random_device rd;
std::mt19937 gen(rd());

std::uniform_int_distribution<> offerTimeDist(0, MAX_OFFER_INTER_ARRIVAL);
std::uniform_int_distribution<> jobTimeDist(0, MAX_JOB_INTER_ARRIVAL);

bool
ReadOneTuple(std::ifstream& strm, std::int64_t& a, std::int64_t& b)
{
	std::string line;
	a = b = 0;
	if (getline(strm, line)) {
		std::istringstream istrm(line);
		istrm >> a;
		istrm >> b;
		return true;
	}
	return false;
}

} // end anonymous namespace

std::shared_ptr<Event>
ResOfferReadEvent::Handle(Time_t curTime, JobScheduler& scheduler)
{
	std::int64_t id;
	std::int64_t offer;
	if (ReadOneTuple(OfferStream, id, offer)) {
		ResOffer_t resOffer(static_cast<ID_t>(id),
								  static_cast<ULong>(offer));
		if (Verbosity > 0) {
			std::cout << "\t\t@" << curTime << " Register offer: <" << id
						 << ", " << offer << ">" << std::endl;
		}
		scheduler.RegisterOffer(resOffer);
	}
	Time_t newTime = curTime + 1; // offerTimeDist(gen);
	return MakeResOfferReadEvent(newTime);
}

std::shared_ptr<Event>
JobArrivalEvent::Handle(Time_t curTime, JobScheduler& scheduler)
{
	std::int64_t dur;
	std::int64_t reqt;
	if (!ReadOneTuple(JobStream, dur, reqt)) {
		return nullptr;
	}
	std::shared_ptr<Job> job = Job::MakeJob(curTime,
		static_cast<Time_t>(dur), static_cast<ULong>(reqt));
	if (Verbosity > 0) {
		std::cout << "\t\t@" << curTime << " Queuing job: " << job->GetId()
					 << " <" << dur << ", " << reqt << ">" << std::endl;
	}
	scheduler.QueueJob(curTime, job);

	Time_t newTime = curTime + 1; // jobTimeDist(gen);
	return MakeJobArrivalEvent(newTime);
}

std::shared_ptr<Event>
JobCompletedEvent::Handle(Time_t curTime, JobScheduler& scheduler)
{
	assert(_job != nullptr);
	ID_t nodeId =  _job->GetRunningOn()->GetId();
	ULong res = _job->GetRequirement();
	if (Verbosity > 0) {
		std::cout << "\t\t@"<< curTime << " <<<<<<Finished job " << _job->GetId()
					 << " on: <" << nodeId << ", " << res << ">" << std::endl;
	}

	scheduler.AccountFinishedJob(_job);

	ResOffer_t resOffer(nodeId, res);
	scheduler.RegisterOffer(resOffer);

	Time_t newTime = curTime + 1; // offerTimeDist(gen);
	return nullptr;
}


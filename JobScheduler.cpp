#include <cstdint>
#include <memory>

#include "Job.h"
#include "JobScheduler.h"
#include "Node.h"
#include "SchedTypes.h"
#include "Event.h"


extern short int Verbosity;

void
JobScheduler::QueueJob(Time_t curTime, std::shared_ptr<Job> job)
{
	_jobQueue.push_back(job);
	push_heap(_jobQueue.begin(), _jobQueue.end(), _queuePolicy);
}

std::shared_ptr<Event>
JobScheduler::PlaceJob(Time_t curTime, std::shared_ptr<Job> job)
{
	std::shared_ptr<Event> newEvent;
	make_heap(_nodeList.begin(), _nodeList.end());
	for (auto node : _nodeList) {
		if (job->PlaceOn(curTime, node)) {
			newEvent = Event::MakeJobCompletedEvent(curTime + job->GetDuration(), job);
			break;
		}
	}
	if (job->IsRunning()) {
		job->MarkWaitState(curTime);
	}
	return newEvent;
}

void
JobScheduler::DumpNodeCapacities(void)
{
	std::cout << "\tNodeCap ";
	for (auto node : _nodeList) {
		node->Dump();
	}
	std::cout << std::endl;
}

void
JobScheduler::DumpJobQueue(void)
{
	std::cout << "\tJobQueue ";
	for (auto job : _jobQueue) {
		job->Dump();
	}
	std::cout << std::endl;
}

/*
 * Verify if any node has enough capacity to host a waiting job.
 * This is a sanity check to make sure that placement algo didn't miss
 * any available host.
 */
void
JobScheduler::VerifyPlacement(Time_t curTime)
{
	for (auto job : _jobQueue) {
		if (!job->IsRunning()) {
			for (auto node : _nodeList) {
				if (job->IsPlaceable(node)) {
					std::cerr << "!!!!!!! Host " << node->GetId()
							    << " didn't accept job " << job->GetId()
								 << std::endl;
				}
			}
		}
	}
}

/*
 * Compute total free capacity in the cluster and print it.
 */
void
JobScheduler::PrintFreeCapacity(Time_t curTime)
{
	ULong capacity = 0;
	for_each (_nodeList.begin(), _nodeList.end(), [&] (std::shared_ptr<Node> node) {
		capacity += node->GetAvailable();
	});
	std::cout << "Free Capacity @ " << curTime << " : " << capacity << std::endl;
}

void
JobScheduler::RunAlgo(Time_t curTime, std::list<std::shared_ptr<Event> >& eventList)
{
	make_heap(_nodeList.begin(), _nodeList.end(), NodeOrdering());
	if (Verbosity > 1) {
		DumpNodeCapacities();
		DumpJobQueue();
	}
	for (auto ji = _jobQueue.begin(); ji != _jobQueue.end(); ) {
		// TODO: Use lambdas.
		std::shared_ptr<Job> job = *ji;
		for (auto node : _nodeList) {
			if (job->PlaceOn(curTime, node)) {
				std::shared_ptr<Event> ev = Event::MakeJobCompletedEvent(curTime + job->GetDuration(), job);
				eventList.push_back(ev);
				job->LogQueueTime(curTime);
				break;
			}
		}

		if (!job->IsRunning()) {
			// If the job was not placed, mark it as waiting.
			job->MarkWaitState(curTime);
			++ji;
		} else {
			// Job is running; remove it from the queue.
			ji = _jobQueue.erase(ji);
		}
	}
}

/*
 * Takes in an incoming resource offering and updates the Node(Host) database.
 * If it is a new resource, we create an entry, otherwise add to the capacity.
 */
void
JobScheduler::RegisterOffer(const ResOffer_t& offer)
{
	ID_t id = offer.first;
	std::map<ID_t, std::shared_ptr<Node> >::iterator it = _nodeTable.find(id);
	if (it == _nodeTable.end()) {
		std::shared_ptr<Node> node(new Node(id));
		_nodeTable[id] = node;
		_nodeList.push_back(node);
	}
	_nodeTable[id]->AddAvailable(offer.second);
}

/*
 * Keep track of jobs finished and their wait times.
 */
void
JobScheduler::AccountFinishedJob(std::shared_ptr<Job> job)
{
	++_jobCount;
	_totalWaitDuration += job->GetQueuedDuration();	
}


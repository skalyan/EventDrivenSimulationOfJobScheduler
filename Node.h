#pragma once

#include "SchedTypes.h"

/*
 * Represents a compute node.
 * - Properties: ID and Available Capacity.
 */
class Node {
public:
	Node(ID_t id, ULong cap = 0) :
      _id(id), _available(cap) {}

	void Dump(void) {
		std::cout << " <" << _id << "," << _available << "> ";
	}

	bool operator < (const std::shared_ptr<Node> other) {
		return _available < other->_available;
	}

	bool Consume(ULong requirement) {
		if (requirement <= _available) {
			_available -= requirement;
			return true;
		}
		return false;
	}
	void Release(ULong used) {
		_available += used;
	}

	ID_t GetId(void) const {
		return _id;
	}

	void AddAvailable(ULong cap) {
		_available += cap;
	}

	ULong GetAvailable() const {
		return _available;
	}
	bool HasAvailable(ULong requirement) {
		assert(requirement > 0);
		return (requirement <= _available);
	}
	#if 0
	bool PlaceJob(std::shared_ptr<Job> job) {
		if (job->GetRequirement() <= _available) {
			Consume(job->GetRequirement());
			return true;
		}
		return false;
	}
	#endif
	
private:
	ID_t  _id;
	ULong _available;
};

class NodeOrdering {
public:
	bool operator()( const std::shared_ptr<Node> a, const std::shared_ptr<Node> b)
	{
		#ifdef DEBUG_LEVEL5
		std::cout << " CompareNodes left " << a->GetAvailable()
					 << " right " << b->GetAvailable() << std::endl;
		#endif
		return a->GetAvailable() > b->GetAvailable();
	}
};

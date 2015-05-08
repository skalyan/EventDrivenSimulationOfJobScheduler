
#include <queue>
#include <ostream>
#include <getopt.h>
#include <unistd.h>

#include "TimeWheel.h"
#include "JobScheduler.h"


JobScheduler::Policy SchedulerPolicy;
short int Verbosity;
std::int64_t NumCycles;

void
ProcessArgs(int argc, char *argv[])
{
	int opt;
	std::string input;

	SchedulerPolicy = JobScheduler::Policy::FCFS;

	// Retrieve the (non-option) argument:
	if ( (argc <= 1) || (argv[argc-1] == NULL) || (argv[argc-1][0] == '-') ) {
		std::cerr << "Usage: ";
		std::cerr << "mesos -c <num_cycles> -p <fcfs/raw/time> -v <verbosity_level>" << std::endl;
		exit(1);
	}

	while ( (opt = getopt(argc, argv, "c:p:v:")) != -1 ) {
		switch ( opt ) {
		case 'c':
			NumCycles = atoi(optarg);
			break;
		case 'v':
			Verbosity = atoi(optarg);
			break;
		case 'p':
			if (!strcasecmp(optarg, "raw")) {
				SchedulerPolicy = JobScheduler::Policy::RawDemand;
			} else if (!strcasecmp(optarg, "time")) {
				SchedulerPolicy = JobScheduler::Policy::TimeWeightedDemand;
			} else if (!strcasecmp(optarg, "fcfs")) {
				SchedulerPolicy = JobScheduler::Policy::FCFS;
			}
			break;
		case '?':
			std::cerr << "Unknown option: '" << optarg << "'!" << std::endl;
			break;
		}
	}
}

int
main(int argc, char *argv[])
{
	int numCycles = 128;
	JobScheduler::Policy policy = JobScheduler::Policy::RawDemand;

	ProcessArgs(argc, argv);

	std::cout << "Running miniMesos simulator for " << NumCycles << std::endl;
	TimeWheel mesoWheel(SchedulerPolicy, NumCycles, Verbosity);

	mesoWheel.Simulate();
}

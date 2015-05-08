CC=/usr/bin/g++
CFLAGS=-g -c -std=c++11

minmesos:
	$(CC) $(CFLAGS) -o Job.o Job.cpp
	$(CC) $(CFLAGS) -o Event.o Event.cpp
	$(CC) $(CFLAGS) -o JobScheduler.o JobScheduler.cpp
	$(CC) $(CFLAGS) -o TimeWheel.o TimeWheel.cpp
	$(CC) -g -std=c++11 -o minmesos Job.o Event.o JobScheduler.o TimeWheel.o MinMesos.cpp
clean:
	rm -f minmesos

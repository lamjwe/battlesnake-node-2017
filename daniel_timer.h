#pragma once
#ifdef WIN32
#include <time.h>
#include <windows.h>
#else
#include <sys/types.h>
#include <sys/times.h>
#include <unistd.h>
#endif

#ifdef DEBUG_OUTPUT
#include <stdio.h>
#endif

class Timer
{
public:

	Timer()
	{
#ifdef WIN32
		time(&start_time);
		GetSystemTime(&ms_time);
#else
		times(&start_time);
		ticksPerSec = sysconf(_SC_CLK_TCK);
		ticksPerSec = 60;
#endif
	}

private:
#ifdef WIN32
	time_t start_time;
	SYSTEMTIME ms_time;
#else
	struct tms start_time;
	int ticksPerSec;
#endif

public:

	int secondsPassed() const
	{
#ifdef WIN32
		time_t currentTime;
		time(&currentTime);
		long long delta = (long long)(currentTime - start_time);
		return (int)delta;
#else
		struct tms currentTime;
		times(&currentTime);
#ifdef DEBUG_OUTPUT
		std::cout << "TPS: " << ticksPerSec << " and time = " << (currentTime.tms_utime - start_time.tms_utime) / ticksPerSec << std::endl;
#endif
		return (currentTime.tms_utime - start_time.tms_utime)/ ticksPerSec;
#endif
	}

	long long millisecondsPassed() const
	{
#ifdef WIN32
		SYSTEMTIME currentTime;
		GetSystemTime(&currentTime);
		return (currentTime.wSecond - ms_time.wSecond) * 1000 + (currentTime.wMilliseconds - ms_time.wMilliseconds);
#endif
		return 0;
	}

	void start()
	{
#ifdef WIN32
		time(&start_time);
		GetSystemTime(&ms_time);
#else
		times(&start_time);
#endif
	}
};


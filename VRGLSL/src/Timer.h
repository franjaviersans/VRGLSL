#ifndef Timer_H
#define Timer_H


#include "Definitions.h"


#ifdef MEASURE_TIME
class TimerManager
{
public:
	TimerManager();
	void Init();
	void Start();
	void Stop();
	double GetTime();
	double GetAverageTime(int count);

private:
	double freq;
	LARGE_INTEGER start_time, end_time;
};
#endif
#endif
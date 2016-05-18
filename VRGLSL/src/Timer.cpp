#include "Timer.h"

#ifdef MEASURE_TIME
TimerManager::TimerManager(){

}

void TimerManager::Init(){
	LARGE_INTEGER temp;
	QueryPerformanceFrequency((LARGE_INTEGER *)&temp);
	freq = ((double)temp.QuadPart) / 1000.0; //convert to the time needed
}

void TimerManager::Start(){
	QueryPerformanceCounter((LARGE_INTEGER *)&start_time);	//set start time
}

void TimerManager::Stop(){
	QueryPerformanceCounter((LARGE_INTEGER *)&end_time);	//set start time
}

double TimerManager::GetTime(){
	return (end_time.QuadPart - start_time.QuadPart) / freq;
}

double TimerManager::GetAverageTime(int count){
	return GetTime() / count;
}
#endif
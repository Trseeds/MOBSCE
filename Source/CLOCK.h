#ifndef CLOCK_H
#define CLOCK_H

#include "TYPES.h"
#include "OBJECTS.h"

typedef struct Clock {
	double DeltaTime;
	uint64 PerfTime;
	uint64 PrevPerfTime;
	uint64 TotalTicks;
	int64 RealTime;
	int64 RealTimeMS;
	double FrameRate;
} Clock;

enum TimerType {
	TMR_T_COUNT_TO,
	TMR_T_WAIT_UNTIL
};

enum TimerUnitType {
	TMR_UT_FRAMES,
	TMR_UT_SECONDS,
	TMR_UT_MILLISECONDS
};

int64 OSGetRealTime();
int64 OSGetRealTimeMS();
int8 TickTimers(Engine* Engine);
int8 KeepTime(Engine* Engine);

#endif
#include "MOBSCE.h"

#ifdef _WIN32
    #include <sys/timeb.h>
    int64 OSGetRealTime()
    {
        struct _timeb TB;
        _ftime_s(&TB);
        return(TB.time);
    }
    int64 OSGetRealTimeMS()
    {
        struct _timeb TB;
        _ftime_s(&TB);
        return((int64)TB.time * 1000 + TB.millitm);
    }
#elif defined(__unix__)
    #include <time.h>
    int64 OSGetRealTime()
    {
        return((int64)time(NULL));
    }
    int64 OSGetRealTimeMS()
    {
        struct timespec TS;
        if(timespec_get(&TS, TIME_UTC))
        {
            return(TS.tv_sec * 1000 + TS.tv_nsec / 1000000);
        }
        return(ERROR_TIME_FAILURE);
    }
#endif

int8 KeepTime(Engine* Engine)
{
    if(Engine)
    {
        Clock* C = &Engine->Clock;
        C->RealTime = OSGetRealTime();
        C->RealTimeMS = OSGetRealTimeMS();
        C->PrevPerfTime = C->PerfTime;
        C->PerfTime = SDL_GetPerformanceCounter();
        C->DeltaTime = (double)((C->PerfTime-C->PrevPerfTime)/(double)SDL_GetPerformanceFrequency());
        C->TotalTicks++;
        C->FrameRate = (double)(1/C->DeltaTime);
        return(RETURN_SUCCESS);
    }
    return(ERROR_INVALID_ENGINE);
}

int8 TickTimers(Engine* Engine) //i've {created {a {monster}}}
{
    if(Engine)
    {
        Timer* T = Engine->Timers;
        int ATM = Engine->Resource.AllocatedTimerMemory;
        for(int i = 0; i < ATM; i++)
        {
            if(T[i].IsUsed)
            {
                Timer* CT = &T[i];
                if(!CT->Expired)
                {
                    if(CT->TimerType == TMR_T_COUNT_TO)
                    {
                        if(CT->UnitType == TMR_UT_FRAMES)
                        {
                            if(CT->Counter >= CT->StopAt)
                            {
                                CT->Expired = true;
                            }
                            else
                            {
                                CT->Counter++;
                            }
                        }
                        if(CT->UnitType == TMR_UT_SECONDS || CT->UnitType == TMR_UT_MILLISECONDS)
                        {
                            if(CT->StopAt <= Engine->Clock.RealTimeMS)
                            {
                                CT->Expired = true;
                            }
                        }
                    }

                    if(CT->TimerType == TMR_T_WAIT_UNTIL)
                    {
                        if(CT->UnitType == TMR_UT_FRAMES)
                        {
                            if(CT->StopAt <= Engine->Clock.TotalTicks)
                            {
                                CT->Expired = true;
                            }
                        }
                        if(CT->UnitType == TMR_UT_SECONDS || CT->UnitType == TMR_UT_MILLISECONDS)
                        {
                            if(CT->StopAt <= Engine->Clock.RealTimeMS)
                            {
                                CT->Expired = true;
                            } //one
                        } //two
                    } //three
                } //four
            } //five
        } //six
        return(RETURN_SUCCESS);
    } //seven
    return(ERROR_INVALID_ENGINE);
} //eight

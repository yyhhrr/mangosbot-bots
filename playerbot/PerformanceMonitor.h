#ifndef _PerformanceMonitor_H
#define _PerformanceMonitor_H
#define PMO_MEMTEST

#include "Common.h"
#include "PlayerbotAIBase.h"

#include <mutex>
#include <chrono>
#include <ctime>

using namespace std;

typedef vector<string> PerformanceStack;

struct PerformanceData
{
    uint32 minTime, maxTime, totalTime, count;
#ifdef CMANGOS
    std::mutex lock;
#endif
};

enum PerformanceMetric
{
    PERF_MON_VALUE,
    PERF_MON_TRIGGER,
    PERF_MON_ACTION,
    PERF_MON_RNDBOT,
    PERF_MON_TOTAL
};

class PerformanceMonitorOperation
{
public:
    PerformanceMonitorOperation(PerformanceData* data, string name, PerformanceStack* stack);
    void finish();

private:
    PerformanceData* data;
    string name;
    PerformanceStack* stack;
#ifdef CMANGOS
    std::chrono::milliseconds started;
#endif
};

class PerformanceMonitor
{
    public:
        PerformanceMonitor();
        virtual ~PerformanceMonitor();
        static PerformanceMonitor& instance()
        {
            static PerformanceMonitor instance;
            return instance;
        }

	public:
        PerformanceMonitorOperation* start(PerformanceMetric metric, string name, PerformanceStack* stack = nullptr);
        PerformanceMonitorOperation* start(PerformanceMetric metric, string name, PlayerbotAI* ai);
        void PrintStats(bool perTick = false,  bool fullStack = false);
        void Reset();
	private:
        map<PerformanceMetric, map<string, PerformanceData*> > data;
#ifdef CMANGOS
		std::mutex lock;
#endif
};


#define sPerformanceMonitor PerformanceMonitor::instance()

#endif

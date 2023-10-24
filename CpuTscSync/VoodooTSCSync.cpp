#include "VoodooTSCSync.h"
#include "CpuTscSync.hpp"

#include <i386/proc_reg.h>
#include <IOKit/IOTimerEventSource.h>

OSDefineMetaClassAndStructors(VoodooTSCSync, IOService)


inline int getThreadCount() {
    unsigned int threads = 0;
    
    asm volatile (
        "mov $0x80000008, %%eax\n"
        "cpuid\n"
        "mov %%ecx, %0\n"
        : "=r" (threads)
        :
        : "%eax", "%ecx"
    );
    
    return (threads & 0xFF) + 1;
}

IOService* VoodooTSCSync::probe(IOService* provider, SInt32* score)
{
    if (!provider) return NULL;
    if (!super::probe(provider, score)) return NULL;

    OSNumber* cpuNumber = OSDynamicCast(OSNumber, provider->getProperty("IOCPUNumber"));
    if (!cpuNumber) return NULL;

    if (getKernelVersion() >= KernelVersion::Monterey) {
        // only attach to the first CPU
        if (cpuNumber->unsigned16BitValue() != 0) return NULL;
    } else {
        // only attach to the last CPU
        uint16_t threadCount = getThreadCount();
        if (cpuNumber->unsigned16BitValue() != threadCount-1) return NULL;
    }

    return this;
}

void VoodooTSCSync::sync_tsc_wrapper(){
    // doesn't sync when wake, wait clock_get_calendar_microtime to sync
    if(CpuTscSyncPlugin::tsc_synced){
        CpuTscSyncPlugin::tsc_adjust_or_reset();
    }
}

bool VoodooTSCSync::start(IOService *provider) {
    if (!IOService::start(provider)) {
        SYSLOG("cputs", "failed to start the parent");
        return false;
    }
    
    if(checkKernelArgument("-cputsnoloop"))return true;
    
    SYSLOG("cputs", "TSC will be synced via timer");
    
    myWorkLoop = getWorkLoop();
            
    // Create IOKit timer
    myTimer = IOTimerEventSource::timerEventSource(this,OSMemberFunctionCast(IOTimerEventSource::Action, this, &VoodooTSCSync::sync_tsc_wrapper));
    
    if (!myTimer)
    {
        DBGLOG("cputs","Failed to create timer event source");
        return false;
    }
    
    if (myWorkLoop->addEventSource(myTimer) != kIOReturnSuccess)
    {
        DBGLOG("cputs","Failed to add timer event source to workloop");
        return false;
    }
    
    myTimer->setTimeoutMS(5000);

    return true;
}

void VoodooTSCSync::stop(IOService *provider)
{

    if (myTimer && checkKernelArgument("-cputsnoloop"))
    {
        myTimer->cancelTimeout();
        myWorkLoop->removeEventSource(myTimer);
        myTimer->release();
        myTimer = NULL;
    }
    
    IOService::stop(provider);
}

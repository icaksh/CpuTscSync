#include "VoodooTSCSync.h"
#include "CpuTscSync.hpp"

#include <i386/proc_reg.h>

OSDefineMetaClassAndStructors(VoodooTSCSync, IOService)


int getThreadCount() {
    unsigned int threads = 0;
    
    asm volatile (
        "mov $0x80000008, %%eax\n"
        "cpuid\n"
        "mov %%ecx, %0\n"
        : "=r" (threads)
        :
        : "%eax", "%ecx"
    );
    
    threads = (threads & 0xFF) + 1;
    
    return threads;
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
        CpuTscSyncPlugin::tsc_adjust_or_reset();
    } else {
        // only attach to the last CPU
        uint16_t threadCount = getThreadCount();
        if (cpuNumber->unsigned16BitValue() != threadCount-1) return NULL;
        CpuTscSyncPlugin::tsc_adjust_or_reset();
    }

    return this;
}

bool VoodooTSCSync::start(IOService *provider) {
    if (!IOService::start(provider)) {
        SYSLOG("cputs", "failed to start the parent");
        return false;
    }

    PMinit();
    provider->joinPMtree(this);
    registerPowerDriver(this, powerStates, arrsize(powerStates));

    return true;
}

IOReturn VoodooTSCSync::setPowerState(unsigned long state, IOService *whatDevice){
    if (!CpuTscSyncPlugin::is_non_legacy_method_used_to_sync()) {
        DBGLOG("cputs", "changing power state to %lu", state);
        if (state == PowerStateOff)
            CpuTscSyncPlugin::reset_sync_flag();
        if (state == PowerStateOn)
            CpuTscSyncPlugin::tsc_adjust_or_reset();
    }

    return kIOPMAckImplied;
}

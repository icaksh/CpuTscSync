//
//  CpuTscSync.hpp
//  CpuTscSync
//
//  Copyright © 2020 lvs1974. All rights reserved.
//

#ifndef kern_cputs_hpp
#define kern_cputs_hpp

#include <Headers/kern_patcher.hpp>
#include <stdatomic.h>


//reg define
#define MSR_IA32_TSC                    0x00000010

enum { kIOPMTracePointWakeCPUs  = 0x23 };

class CpuTscSyncPlugin {
public:
	void init();
    static _Atomic(bool)     tsc_synced;
	
private:
    _Atomic(bool) kernel_routed = false;
    static _Atomic(bool)     kernel_is_awake;
    
private:
	/**
	 *  Trampolines for original resource load callback
	 */
    mach_vm_address_t orgIOHibernateSystemHasSlept {0};
    mach_vm_address_t orgIOHibernateSystemWake {0};
    mach_vm_address_t orgIOPMrootDomain_tracePoint {0};
    mach_vm_address_t org_clock_get_calendar_microtime {0};
    
	/**
	 *  Hooked functions
	 */
    static IOReturn IOHibernateSystemHasSlept();
    static IOReturn IOHibernateSystemWake();
    static void     IOPMrootDomain_tracePoint( void *that, uint8_t point );
    static void     clock_get_calendar_microtime(clock_sec_t *secs, clock_usec_t *microsecs);
 	
	/**
	 *  Patch kernel
	 *
	 *  @param patcher KernelPatcher instance
	 */
	void processKernel(KernelPatcher &patcher);
    
    
    static void stamp_tsc(void *tscp);

public:
    static void tsc_adjust_or_reset();
};

#endif /* kern_cputs_hpp */

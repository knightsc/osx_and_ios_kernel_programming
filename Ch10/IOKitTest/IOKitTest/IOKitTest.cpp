#include "IOKitTest.hpp"
#include <IOKit/IOLib.h>

#define super IOService

OSDefineMetaClassAndStructors(com_osxkernel_driver_IOKitTest, IOService)

enum {
    kOffPowerState,
    kStandbyPowerState,
    kIdlePowerState,
    kOnPowerState,
    kNumPowerStates
};

//
// Actually running this code and seeing the logs is really helping in
// soidifying the understanding of when methods will be called and
// what the power states are being passed in
//
//IOKitTest::start
//IOKitTest::powerStateWillChangeTo(0x3)
//IOKitTest::setPowerState(0x3)
//IOKitTest::powerStateDidChangeTo(0x3)
//IOKitTest::powerChangeDone(0x0)
//IOKitTest::powerStateWillChangeTo(0x2)
//IOKitTest::setPowerState(0x2)
//IOKitTest::powerStateDidChangeTo(0x2)
//IOKitTest::powerChangeDone(0x3)
//IOKitTest::powerStateWillChangeTo(0x1)
//IOKitTest::setPowerState(0x1)
//IOKitTest::powerStateDidChangeTo(0x1)
//IOKitTest::powerChangeDone(0x2)
//IOKitTestUserClient::sReadData
//IOKitTest waiting on power transition
//IOKitTest::powerStateWillChangeTo(0x3)
//IOKitTest::setPowerState(0x3)
//IOKitTest::powerStateDidChangeTo(0x3)
//IOKitTest::powerChangeDone(0x1)
//IOKitTest reading some data
//IOKitTest::stop
//IOKitTest::free



static IOPMPowerState gPowerStates[kNumPowerStates] = {
    // kOffPowerState
    {kIOPMPowerStateVersion1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    // kStandbyPowerState
    {kIOPMPowerStateVersion1, kIOPMPowerOn, kIOPMPowerOn, kIOPMPowerOn, 0, 0, 0, 0, 0, 0, 0},
    // kIdlePowerState
    {kIOPMPowerStateVersion1, kIOPMPowerOn, kIOPMPowerOn, kIOPMPowerOn, 0, 0, 0, 0, 0, 0, 0},
    // kOnPowerState
    {kIOPMPowerStateVersion1, (kIOPMPowerOn | kIOPMDeviceUsable), kIOPMPowerOn, kIOPMPowerOn, 0, 0, 0, 0, 0, 0, 0, 0}
};

bool com_osxkernel_driver_IOKitTest::start (IOService *provider)
{
    IOLog("IOKitTest::start\n");
    
    if (!super::start(provider)) {
        return false;
    }
    
    // Create a lock for driver/power management synchronization
    m_lock = IOLockAlloc();
    if (!m_lock) {
        return false;
    }
    
    PMinit();
    provider->joinPMtree(this);
    makeUsable(); // This sets the private power state to the highest level
    changePowerStateTo(kOffPowerState); // This sets the public power state to the lowest level
    registerPowerDriver(this, gPowerStates, kNumPowerStates);
    
    setIdleTimerPeriod(60);
    
    registerService();
    
    return true;
}

void com_osxkernel_driver_IOKitTest::stop (IOService *provider)
{
    IOLog("IOKitTest::stop\n");
    PMstop();
    super::stop(provider);
}

void com_osxkernel_driver_IOKitTest::free()
{
    IOLog("IOKitTest::free\n");

    if (m_lock) {
        IOLockFree(m_lock);
    }
    
    super::free();
}



IOReturn com_osxkernel_driver_IOKitTest::setPowerState(unsigned long powerStateOrdinal, IOService *whatDevice)
{
    IOLog("IOKitTest::setPowerState(0x%lx)\n", powerStateOrdinal);
    
    if (powerStateOrdinal < m_devicePowerState) {
        m_devicePowerState = powerStateOrdinal;
    }
    
    switch (powerStateOrdinal) {
        case kOffPowerState:
        case kStandbyPowerState:
        case kIdlePowerState:
            // Wait for outstanding IO to complete before putting device into a
            // lowe power state
            IOLockLock(m_lock);
            while (m_outstandIO != 0) {
                IOLockSleep(m_lock, &m_outstandIO, THREAD_UNINT);
            }
            IOLockUnlock(m_lock);
            
            // Prepare hardware for sleep
            break;
    }
    
    if (powerStateOrdinal > m_devicePowerState) {
        m_devicePowerState = powerStateOrdinal;
    }
    
    return kIOPMAckImplied;
}

IOReturn com_osxkernel_driver_IOKitTest::powerStateWillChangeTo(IOPMPowerFlags capabilities, unsigned long stateNumber, IOService *whatDevice)
{
    IOLog("IOKitTest::powerStateWillChangeTo(0x%lx)\n", stateNumber);
    return IOPMAckImplied;
}

IOReturn com_osxkernel_driver_IOKitTest::powerStateDidChangeTo(IOPMPowerFlags capabilities, unsigned long stateNumber, IOService *whatDevice)
{
    IOLog("IOKitTest::powerStateDidChangeTo(0x%lx)\n", stateNumber);
    return IOPMAckImplied;
}

void com_osxkernel_driver_IOKitTest::powerChangeDone(unsigned long stateNumber)
{
    // Wake up threads waiting on power state change
    IOLog("IOKitTest::powerChangeDone(0x%lx)\n", stateNumber);
    IOLockWakeup(m_lock, &m_devicePowerState, false);
}

// Sample Device Operation
IOReturn com_osxkernel_driver_IOKitTest::myReadDataFromDevice()
{
    // Ensure the device is in the on power state
    IOLockLock(m_lock);
    
    if (!activityTickle(kIOPMSuperclassPolicy1, kOnPowerState)) {
        // wait untilt he device transitions to the on state
        while (m_devicePowerState != kOnPowerState) {
            IOLog("IOKitTest waiting on power transition\n");
            IOLockSleep(m_lock, &m_devicePowerState, THREAD_UNINT);
        }
    }
    
    m_outstandIO += 1;
    
    IOLockUnlock(m_lock);
    
    // Perform the actual device read ...
    IOLog("IOKitTest reading some data\n");
    
    IOLockLock(m_lock);
    
    m_outstandIO -= 1;
    IOLockWakeup(m_lock, &m_outstandIO, false);
    
    IOLockUnlock(m_lock);
    
    return kIOReturnSuccess;
}

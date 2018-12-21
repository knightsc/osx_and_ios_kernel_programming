#include <IOKit/IOLib.h>
#include <IOKit/IOService.h>

#include <IOKit/usb/IOUSBHostInterface.h>

#include <IOKit/serial/IOSerialKeys.h>
#include <IOKit/serial/IOModemSerialStreamSync.h>

#include "MySerialDriver.h"

#define super IOSerialDriverSync

OSDefineMetaClassAndStructors(MySerialDriver, IOSerialDriverSync);

bool MySerialDriver::createSerialStream()
{
    IOSerialStreamSync *pChild;
    bool result;
    
    // Instantiate the child driver object
    pChild = new IOModemSerialStreamSync;
    if (!pChild) {
        return false;
    }
    
    // Initialize the child driver
    result = pChild->init(0, 0);
    if (!result) {
        goto bail;
    }
    
    // Attach pChild as a child device of ourself
    result = pChild->attach(this);
    if (!result) {
        goto bail;
    }

    // Setup the properties used when naming the device file in /dev
    pChild->setProperty(kIOTTYBaseNameKey, "my_serial");
    pChild->setProperty(kIOTTYSuffixKey, "");
    
    // Allow matching of drivers that use pChild as their provider class
    pChild->registerService();
    
    // Fall-through on success
bail:
    pChild->release();
    return result;
}

IOService *MySerialDriver::probe(IOService *provider, SInt32 *score)
{
    IOService *res;
    
    res = super::probe(provider, score);
    
    return res;
    
}

bool MySerialDriver::start(IOService *provider)
{
    if (!super::start(provider)) {
        return false;
    }
    
    fInterface = OSDynamicCast(IOUSBHostInterface, provider);
    if(!fInterface) {
        return false;
    }
    
    fWorkLoop = getWorkLoop();
    if (!fWorkLoop) {
        return false;
    }
    
    fCommandGate = IOCommandGate::commandGate(this);
    if (!fCommandGate) {
        return false;
    }
    
    if (fWorkLoop->addEventSource(fCommandGate) != kIOReturnSuccess) {
        return false;
    }
    
    if (!createSerialStream()) {
        return false;
    }
    
    fInterface->retain();
    fWorkLoop->retain();
    fCommandGate->enable();

    return true;
}

void MySerialDriver::stop(IOService *provider)
{
    fCommandGate->disable();
    
    if (fWorkLoop) {
        fWorkLoop->release();
        fWorkLoop = NULL;
    }
    
    if (fInterface) {
        fInterface->close(this);
        fInterface->release();
        fInterface = NULL;
    }
    
    super::stop(provider);
}

IOReturn MySerialDriver::acquirePort(bool sleep, void *refCon)
{
    IOReturn ret;
    if (!fWorkLoop) {
        return kIOReturnOffline;
    }
    
    retain();
    ret = fCommandGate->runAction(acquirePortAction, (void *)sleep, (void *)refCon);
    release();
    
    return ret;
}

IOReturn MySerialDriver::acquirePortAction(OSObject *owner, void *arg0, void *arg1, void *, void *)
{
    return ((MySerialDriver *)owner)->acquirePortGated((bool)arg0, arg1);
}

IOReturn MySerialDriver::acquirePortGated(bool sleep, void *refCon)
{
    UInt32 state;
    IOReturn ret;
    
    while (m_currentState & PD_S_ACQUIRED) {
        if (sleep == false) {
            return kIOReturnExclusiveAccess;
        }
        
        state = 0;
        ret = watchState(&state, PD_S_ACQUIRED, refCon);
        if (ret != kIOReturnSuccess) {
            return ret;
        }
    }
    
    setState(PD_S_ACQUIRED, 0xFFFFFFFF, refCon);
    
    // Serial port has been acquired, perform further initialization
    
    return kIOReturnSuccess;
}

IOReturn MySerialDriver::releasePort(void *refCon)
{
    IOReturn ret = kIOReturnSuccess;
    
    ret = fCommandGate->runAction(releasePortAction);
    
    return ret;
}

IOReturn MySerialDriver::releasePortAction(OSObject *owner, void *arg0, void *, void *, void *)
{
    // Call through to the method releasePortGated()
    return ((MySerialDriver *)owner)->releasePortGated(arg0);
}

IOReturn MySerialDriver::releasePortGated(void *refCon)
{
    // Return an error if trying to release a port that hasn’t been acquired
    if ((m_currentState & PD_S_ACQUIRED) == 0) {
        return kIOReturnNotOpen;
    }
    
    // Clear the entire state word, which also deactivates the port
    setState(0, 0xFFFFFFFF, refCon);
    
    return kIOReturnSuccess;
}

UInt32 MySerialDriver::getState(void *refCon)
{
    UInt32 currState;
    
    retain();
    currState = fCommandGate->runAction(getStateAction);
    release();
    
    return currState;
}

IOReturn MySerialDriver::getStateAction(OSObject *owner, void *, void *, void *, void *)
{
    UInt32 newState;
    
    newState = ((MySerialDriver *)owner)->getStateGated();
    
    return newState;
}

UInt32 MySerialDriver::getStateGated()
{
    UInt32 state;
    
    state = m_currentState;
    
    return state;
}

IOReturn MySerialDriver::setState(UInt32 state, UInt32 mask, void *refCon)
{
    IOReturn ret = kIOReturnSuccess;

    retain();
    ret = fCommandGate->runAction(setStateAction, (void *)&state, (void *)&mask);
    release();
    
    return ret;
}

IOReturn MySerialDriver::setStateAction(OSObject *owner, void *arg0, void *arg1, void *, void *)
{
    return ((MySerialDriver *)owner)->setStateGated((UInt32 *)arg0, (UInt32 *)arg1);
}

IOReturn MySerialDriver::setStateGated(UInt32 *pState, UInt32 *pMask)
{
    UInt32 state = *pState;
    UInt32 mask = *pMask;
    UInt32 newState;
    UInt32 deltaState;
    
    // Verify that the serial port has been acquired or is being acquired by this call
    if ((m_currentState & PD_S_ACQUIRED) || (state & PD_S_ACQUIRED)) {
        // Compute the new state
        newState = (m_currentState & ~mask) | (state & mask);
        // Determine the mask of changed state bits
        deltaState = newState ^ m_currentState;
        // Set the new state
        m_currentState = newState;
        // If any state that is being observed by a thread in watchState() has changed,
        // wake up all threads asleep on watchState()
        if (deltaState & m_watchStateMask) {
            // Reset watchStateMask; it will be regenerated as each watchStateGated()
            // sleeps
            m_watchStateMask = 0;
            fCommandGate->commandWakeup((void*)&m_currentState);
        }
        return kIOReturnSuccess;
    }
    return kIOReturnNotOpen;
}

IOReturn MySerialDriver::watchState(UInt32 *state, UInt32 mask, void *refCon)
{
    IOReturn ret;
    
    if (!state) {
        return kIOReturnBadArgument;
    }
    
    if (!mask) {
        return kIOReturnSuccess;
    }
    
    retain();
    ret = fCommandGate->runAction(watchStateAction, (void *)state, (void *)&mask);
    release();

    return ret;
}

IOReturn MySerialDriver::watchStateAction(OSObject *owner, void *arg0, void *arg1, void *, void *)
{
    return ((MySerialDriver *)owner)->watchStateGated((UInt32 *)arg0, (UInt32 *)arg1);
}

IOReturn MySerialDriver::watchStateGated(UInt32 *state, UInt32 *pMask)
{
    UInt32 mask = *pMask;
    UInt32 watchState;
    bool autoActiveBit = false;
    IOReturn ret;
    
    // Abort if the serial port has not been acquired
    if ((m_currentState & PD_S_ACQUIRED) == 0) {
        return kIOReturnNotOpen;
    }
    
    watchState = *state;
    // If the caller is not waiting on the acquired or active state, register
    // interest in the active state so that we can abort if the serial port closes.
    if ((mask & (PD_S_ACQUIRED | PD_S_ACTIVE)) == 0) {
        watchState &= ~PD_S_ACTIVE;
        mask |= PD_S_ACTIVE;
        autoActiveBit = true;
    }
    
    while (true) {
        // Check port state for any bits that match the watchState value
        // NB. the '^ ~' is a XNOR and tests for equality of bits.
        UInt32 matchedStates = (watchState ^ ~m_currentState) & mask;
        if (matchedStates) {
            *state = m_currentState;
            // Abort if the serial port was closed and the caller didn't watch
            // PD_S_ACTIVE
            if (autoActiveBit && (matchedStates & PD_S_ACTIVE)) {
                return kIOReturnIOError;
            } else {
                return kIOReturnSuccess;
            }
        }
        
        // Add the bits we are sleeping on to watchStateMask
        m_watchStateMask |= mask;
        // Sleep until the serial port state changes
        ret = fCommandGate->commandSleep((void *)&m_currentState);
    
        if (ret == THREAD_INTERRUPTED) {
            return kIOReturnAborted;
        }
    }
    
    return kIOReturnSuccess;
}

UInt32 MySerialDriver::nextEvent(void *refCon)
{
    return kIOReturnSuccess;
}

IOReturn MySerialDriver::executeEvent(UInt32 event, UInt32 data, void *refCon)
{
    return kIOReturnSuccess;
}

IOReturn MySerialDriver::requestEvent(UInt32 event, UInt32 *data, void *refCon)
{
    return kIOReturnSuccess;
}

IOReturn MySerialDriver::enqueueEvent(UInt32 event, UInt32 data, bool sleep, void *refCon)
{
    return kIOReturnSuccess;
}

IOReturn MySerialDriver::dequeueEvent(UInt32 *event, UInt32 *data, bool sleep, void *refCon)
{
    return kIOReturnSuccess;
}

IOReturn MySerialDriver::enqueueData(UInt8 *buffer, UInt32 size, UInt32 *count, bool sleep, void *refCon)
{
    return kIOReturnSuccess;
}

IOReturn MySerialDriver::dequeueData(UInt8 *buffer, UInt32 size, UInt32 *count, UInt32 min, void *refCon)
{
    return kIOReturnSuccess;
}

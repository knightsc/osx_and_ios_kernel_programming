#include "IOKitTestUserClient.hpp"
#include <IOKit/IOLib.h>

// Define the superclass.
#define super IOUserClient

OSDefineMetaClassAndStructors(com_osxkernel_driver_IOKitTestUserClient, IOUserClient)

const IOExternalMethodDispatch
com_osxkernel_driver_IOKitTestUserClient::sMethods[kTestUserClientMethodCount] =
{
    // kTestUserClientStartTimer   (void)
    { sStartTimer, 0, 0, 0, 0 },
    
    // kTestUserClientStopTimer   (void)
    { sStopTimer, 0, 0, 0, 0 },
    
    // kTestUserClientGetElapsedTimerTime (uint32_t* timerValue)
    { sGetElapsedTimerTime, 0, 0, 1, 0 },
    
    // kTestUserClientGetElapsedTimerValue (TimerValue* timerValue)
    { sGetElapsedTimerValue, 0, 0, 0, sizeof(TimerValue) },
    
    // kTestUserClientDelayForMs   (uint32_t milliseconds)
    { sDelayForMs, 1, 0, 0, 0 },
    
    // kTestUserClientDelayForTime  (const TimerValue* timerValue)
    { sDelayForTime, 0, sizeof(TimerValue), 0, 0 }
};

bool com_osxkernel_driver_IOKitTestUserClient::initWithTask (task_t owningTask, void* securityToken, UInt32 type, OSDictionary* properties)
{
    if (!owningTask) {
        return false;
    }
    
    if (! super::initWithTask(owningTask, securityToken , type, properties)) {
        return false;
    }
    
    m_task = owningTask;
    
    // Optional:  Determine whether the calling process has admin privileges.
    IOReturn ret = clientHasPrivilege(securityToken, kIOClientPrivilegeAdministrator);
    if ( ret == kIOReturnSuccess ) {
        // m_taskIsAdmin = true;
    }
    
    return true;
}

void com_osxkernel_driver_IOKitTestUserClient::free (void)
{
    super::free();
}


bool com_osxkernel_driver_IOKitTestUserClient::start (IOService* provider)
{
    if (! super::start(provider)) {
        return false;
    }
    
    m_driver = OSDynamicCast(com_osxkernel_driver_IOKitTest, provider);
    
    if (!m_driver) {
        return false;
    }
    
    return true;
}

IOReturn com_osxkernel_driver_IOKitTestUserClient::clientClose (void)
{
    terminate();
    return kIOReturnSuccess;
}

void com_osxkernel_driver_IOKitTestUserClient::stop (IOService *provider)
{
    super::stop(provider);
}

IOReturn com_osxkernel_driver_IOKitTestUserClient::externalMethod (uint32_t selector, IOExternalMethodArguments* arguments,
                IOExternalMethodDispatch* dispatch, OSObject* target,
                void* reference)
{
    // Ensure the requested control selector is within range.
    if (selector >= kTestUserClientMethodCount) {
        return kIOReturnUnsupported;
    }
    
    dispatch = (IOExternalMethodDispatch*)&sMethods[selector];
    target = this;
    reference = NULL;
    
    return super::externalMethod(selector, arguments, dispatch, target, reference);
}

IOReturn com_osxkernel_driver_IOKitTestUserClient::sStartTimer(OSObject *target, void *reference, IOExternalMethodArguments *args)
{
    IOLog("IOKitTestUserClient::sStartTimer\n");
    return KERN_SUCCESS;
}

IOReturn com_osxkernel_driver_IOKitTestUserClient::sStopTimer(OSObject *target, void *reference, IOExternalMethodArguments *args)
{
    IOLog("IOKitTestUserClient::sStopTimer\n");
    return KERN_SUCCESS;
}

IOReturn com_osxkernel_driver_IOKitTestUserClient::sGetElapsedTimerTime(OSObject *target, void *reference, IOExternalMethodArguments *args)
{
    IOLog("IOKitTestUserClient::sGetElapsedTimerTime\n");
    return KERN_SUCCESS;
}

IOReturn com_osxkernel_driver_IOKitTestUserClient::sGetElapsedTimerValue(OSObject *target, void *reference, IOExternalMethodArguments *args)
{
//    com_osxkernel_driver_IOKitTestUserClient* me;
//    uint32_t timerTime;
//    IOReturn result;
    
//    me = (com_osxkernel_driver_IOKitTestUserClient*)target;
//    // Call the method that implements the operation.
//    result = me->getElapsedTimerTime(&timerTime);
//    // Return the scalar result of the operation to the calling process.
//    arguments->scalarOutput[0] = timerTime;
//    return result;
    
    IOLog("IOKitTestUserClient::sGetElapsedTimerValue\n");
    return KERN_SUCCESS;
}

IOReturn com_osxkernel_driver_IOKitTestUserClient::sDelayForMs(OSObject *target, void *reference, IOExternalMethodArguments *args)
{
    IOLog("IOKitTestUserClient::sDelayForMs\n");
    return KERN_SUCCESS;
}

IOReturn com_osxkernel_driver_IOKitTestUserClient::sDelayForTime(OSObject *target, void *reference, IOExternalMethodArguments *args)
{
//    com_osxkernel_driver_IOKitTestUserClient* me;
//    me = (com_osxkernel_driver_IOKitTestUserClient*)target;
//    
//    return me->delayForTime((TimerValue*)arguments->structureInput);
    
    IOLog("IOKitTestUserClient::sDelayForTime\n");
    return KERN_SUCCESS;
}

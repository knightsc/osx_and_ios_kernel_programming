#include <IOKit/IOLib.h>

#include "IOKitTestUserClient.h"

#define super IOUserClient

OSDefineMetaClassAndStructors(IOKitTestUserClient, IOUserClient)

const IOExternalMethodDispatch
IOKitTestUserClient::sMethods[kTestUserClientMethodCount] =
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

bool IOKitTestUserClient::initWithTask(task_t owningTask, void *securityToken, UInt32 type, OSDictionary *properties)
{
    if (!owningTask) {
        return false;
    }
    
    if (!super::initWithTask(owningTask, securityToken , type, properties)) {
        return false;
    }
    
    m_task = owningTask;
    
    // Optional:  Determine whether the calling process has admin privileges.
    IOReturn ret = clientHasPrivilege(securityToken, kIOClientPrivilegeAdministrator);
    if (ret == kIOReturnSuccess) {
        //m_taskIsAdmin = true;
    }
    
    return true;
}

void IOKitTestUserClient::free(void)
{
    super::free();
}


bool IOKitTestUserClient::start(IOService *provider)
{
    if (!super::start(provider)) {
        return false;
    }
    
    m_driver = OSDynamicCast(IOKitTest, provider);
    
    if (!m_driver) {
        return false;
    }
    
    return true;
}

IOReturn IOKitTestUserClient::clientClose(void)
{
    terminate();
    return kIOReturnSuccess;
}

void IOKitTestUserClient::stop(IOService *provider)
{
    super::stop(provider);
}

IOReturn IOKitTestUserClient::externalMethod(uint32_t selector, IOExternalMethodArguments *arguments,
                IOExternalMethodDispatch *dispatch, OSObject *target,
                void *reference)
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

IOReturn IOKitTestUserClient::sStartTimer(OSObject *target, void *reference, IOExternalMethodArguments *args)
{
    IOLog("IOKitTestUserClient::sStartTimer\n");
    return KERN_SUCCESS;
}

IOReturn IOKitTestUserClient::sStopTimer(OSObject *target, void *reference, IOExternalMethodArguments *args)
{
    IOLog("IOKitTestUserClient::sStopTimer\n");
    return KERN_SUCCESS;
}

IOReturn IOKitTestUserClient::sGetElapsedTimerTime(OSObject *target, void *reference, IOExternalMethodArguments *args)
{
    IOLog("IOKitTestUserClient::sGetElapsedTimerTime\n");
    return KERN_SUCCESS;
}

IOReturn IOKitTestUserClient::sGetElapsedTimerValue(OSObject *target, void *reference, IOExternalMethodArguments *args)
{
//    IOKitTestUserClient* me;
//    uint32_t timerTime;
//    IOReturn result;
    
//    me = (IOKitTestUserClient *)target;
//    // Call the method that implements the operation.
//    result = me->getElapsedTimerTime(&timerTime);
//    // Return the scalar result of the operation to the calling process.
//    arguments->scalarOutput[0] = timerTime;
//    return result;
    
    IOLog("IOKitTestUserClient::sGetElapsedTimerValue\n");
    return KERN_SUCCESS;
}

IOReturn IOKitTestUserClient::sDelayForMs(OSObject *target, void *reference, IOExternalMethodArguments *args)
{
    IOLog("IOKitTestUserClient::sDelayForMs\n");
    return KERN_SUCCESS;
}

IOReturn IOKitTestUserClient::sDelayForTime(OSObject *target, void *reference, IOExternalMethodArguments *args)
{
//    IOKitTestUserClient* me;
//    me = (IOKitTestUserClient *)target;
//    
//    return me->delayForTime((TimerValue*)arguments->structureInput);
    
    IOLog("IOKitTestUserClient::sDelayForTime\n");
    return KERN_SUCCESS;
}

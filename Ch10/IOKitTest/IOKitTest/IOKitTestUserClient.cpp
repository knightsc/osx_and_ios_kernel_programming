#include <IOKit/IOLib.h>

#include "IOKitTestUserClient.h"

// Define the superclass.
#define super IOUserClient

OSDefineMetaClassAndStructors(IOKitTestUserClient, IOUserClient)

const IOExternalMethodDispatch
IOKitTestUserClient::sMethods[kTestUserClientMethodCount] =
{
    // kTestUserClientReadData   (void)
    { sReadData, 0, 0, 0, 0 }
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
    if ( ret == kIOReturnSuccess ) {
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

IOReturn IOKitTestUserClient::sReadData(OSObject *target, void *reference, IOExternalMethodArguments *args)
{
    IOLog("IOKitTestUserClient::sReadData\n");

    IOKitTestUserClient *me;
    me = (IOKitTestUserClient *)target;
    me->m_driver->myReadDataFromDevice();
    
    return KERN_SUCCESS;
}

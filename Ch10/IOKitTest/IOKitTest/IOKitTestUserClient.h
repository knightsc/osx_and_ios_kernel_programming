#ifndef IOKITTESTUSERCLIENT_H
#define IOKITTESTUSERCLIENT_H

#include <IOKit/IOUserClient.h>

#include "IOKitTest.h"
#include "TestDriverInterface.h"

#define IOKitTestUserClient com_osxkernel_driver_IOKitTestUserClient

class IOKitTestUserClient : public IOUserClient
{
    OSDeclareDefaultStructors(IOKitTestUserClient)
    
private:
    task_t m_task;
    IOKitTest *m_driver;
    
public:
    virtual bool initWithTask(task_t owningTask, void *securityToken, UInt32 type, OSDictionary *properties) APPLE_KEXT_OVERRIDE;
    virtual bool start(IOService *provider) APPLE_KEXT_OVERRIDE;
    virtual IOReturn clientClose(void) APPLE_KEXT_OVERRIDE;
    virtual void stop(IOService *provider) APPLE_KEXT_OVERRIDE;
    virtual void free(void) APPLE_KEXT_OVERRIDE;
    
    IOReturn externalMethod(uint32_t selector, IOExternalMethodArguments *args, IOExternalMethodDispatch *dispatch,
                            OSObject *target, void *reference) APPLE_KEXT_OVERRIDE;
    
protected:
    static const IOExternalMethodDispatch sMethods[kTestUserClientMethodCount];
    
    static IOReturn sReadData(OSObject *target, void *reference, IOExternalMethodArguments *args);
};

#endif /* IOKITTESTUSERCLIENT_H */

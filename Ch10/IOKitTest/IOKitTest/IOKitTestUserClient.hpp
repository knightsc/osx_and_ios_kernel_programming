#include <IOKit/IOUserClient.h>
#include "IOKitTest.hpp"
#include "TestDriverInterface.h"

#ifndef IOKitTestUserClient_hpp
#define IOKitTestUserClient_hpp

class com_osxkernel_driver_IOKitTestUserClient : public IOUserClient
{
    OSDeclareDefaultStructors(com_osxkernel_driver_IOKitTestUserClient)
    
private:
    task_t                          m_task;
    com_osxkernel_driver_IOKitTest* m_driver;
    
public:
    virtual bool initWithTask (task_t owningTask, void* securityToken, UInt32 type, OSDictionary* properties) override;
    virtual bool start (IOService* provider) override;
    virtual IOReturn clientClose (void) override;
    virtual void stop (IOService* provider) override;
    virtual void free (void) override;
    
    IOReturn externalMethod(uint32_t selector, IOExternalMethodArguments *args, IOExternalMethodDispatch *dispatch,
                            OSObject *target, void *reference) override;
    
protected:
    static const IOExternalMethodDispatch sMethods[kTestUserClientMethodCount];
    
    static IOReturn sReadData(OSObject *target, void *reference, IOExternalMethodArguments *args);
};

#endif /* IOKitTestUserClient_hpp */

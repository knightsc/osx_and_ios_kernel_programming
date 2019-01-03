#ifndef MYDEBUGDRIVER_H
#define MYDEBUGDRIVER_H

#include <IOKit/IOLib.h>
#include <IOKit/IOService.h>
#include <IOKit/IOTimerEventSource.h>

#define MyDebugDriver com_osxkernel_MyDebugDriver

class MyDebugDriver : public IOService
{
    OSDeclareDefaultStructors(MyDebugDriver);
    
public:
    
    virtual bool init(OSDictionary *dict) APPLE_KEXT_OVERRIDE;
    virtual bool start(IOService *provider) APPLE_KEXT_OVERRIDE;
    virtual void stop(IOService *provider) APPLE_KEXT_OVERRIDE;
    
    void testFunc1(UInt32 arg1, UInt32 arg2, UInt32 arg3, UInt32 arg4);
    void testFunc2(UInt32 arg1, UInt32 arg2, UInt32 arg3, UInt32 arg4);
    void testFunc3(UInt32 arg1, UInt32 arg2, UInt32 arg3, UInt32 arg4);
    
    static void timerFired(OSObject *owner, IOTimerEventSource *sender);
    
private:
    
    IOTimerEventSource *fTimer;
    int fVariable1;
    int fVariable2;
};

#endif /* MYDEBUGDRIVER_H */

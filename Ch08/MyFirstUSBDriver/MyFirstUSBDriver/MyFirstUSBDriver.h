#ifndef MYFIRSTUSBDRIVER_H
#define MYFIRSTUSBDRIVER_H

#include <IOKit/IOService.h>

#define MyFirstUSBDriver com_osxkernel_MyFirstUSBDriver

class MyFirstUSBDriver : public IOService
{
    OSDeclareDefaultStructors(MyFirstUSBDriver)
    
public:
    virtual bool init(OSDictionary *propTable) APPLE_KEXT_OVERRIDE;
    
    virtual IOService *probe(IOService *provider, SInt32 *score) APPLE_KEXT_OVERRIDE;
    virtual bool attach(IOService *provider) APPLE_KEXT_OVERRIDE;
    virtual void detach(IOService *provider) APPLE_KEXT_OVERRIDE;
    virtual bool start(IOService *provider) APPLE_KEXT_OVERRIDE;
    virtual void stop(IOService *provider) APPLE_KEXT_OVERRIDE;
    virtual bool terminate(IOOptionBits options = 0) APPLE_KEXT_OVERRIDE;
};

#endif /* MYFIRSTUSBDRIVER_H */

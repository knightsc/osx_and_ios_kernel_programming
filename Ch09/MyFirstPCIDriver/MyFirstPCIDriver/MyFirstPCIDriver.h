#ifndef MYFIRSTPCIDRIVER_H
#define MYFIRSTPCIDRIVER_H

#include <IOKit/IOService.h>
#include <IOKit/pci/IOPCIDevice.h>

#define MyFirstPCIDriver com_osxkernel_MyFirstPCIDriver

class MyFirstPCIDriver : public IOService
{
    OSDeclareDefaultStructors(MyFirstPCIDriver);
    
private:
    IOPCIDevice *fPCIDevice;
    
public:
    virtual bool start(IOService *provider) APPLE_KEXT_OVERRIDE;
    virtual void stop(IOService *provider) APPLE_KEXT_OVERRIDE;
};

#endif /* MYFIRSTPCIDRIVER_H */

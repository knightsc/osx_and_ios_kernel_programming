#ifndef IOKITTEST_H
#define IOKITTEST_H

#include <IOKit/IOService.h>

#define IOKitTest com_osxkernel_driver_IOKitTest

class IOKitTest : public IOService
{
    OSDeclareDefaultStructors(IOKitTest)
    
private:
    IOLock *m_lock;
    unsigned long m_devicePowerState;
    SInt32 m_outstandIO;
    
protected:
    virtual void powerChangeDone(unsigned long stateNumber) APPLE_KEXT_OVERRIDE;
    virtual IOReturn powerStateWillChangeTo(IOPMPowerFlags capabilities, unsigned long stateNumber, IOService *whatDevice) APPLE_KEXT_OVERRIDE;
    virtual IOReturn powerStateDidChangeTo(IOPMPowerFlags capabilities, unsigned long stateNumber, IOService *whatDevice) APPLE_KEXT_OVERRIDE;
    
public:
    virtual void free(void) APPLE_KEXT_OVERRIDE;
    virtual bool start(IOService *provider) APPLE_KEXT_OVERRIDE;
    virtual void stop(IOService *provider) APPLE_KEXT_OVERRIDE;
    virtual IOReturn setPowerState(unsigned long powerStateOrdinal, IOService *whatDevice) APPLE_KEXT_OVERRIDE;
    
    IOReturn myReadDataFromDevice();
};

#endif /* IOKITTEST_H */

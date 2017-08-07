#include <IOKit/IOService.h>

#ifndef IOKitTest_hpp
#define IOKitTest_hpp

class com_osxkernel_driver_IOKitTest : public IOService
{
    OSDeclareDefaultStructors(com_osxkernel_driver_IOKitTest)
    
private:
    IOLock* m_lock;
    unsigned long m_devicePowerState;
    SInt32 m_outstandIO;
    
protected:
    virtual void powerChangeDone (unsigned long stateNumber) override;
    virtual IOReturn powerStateWillChangeTo(IOPMPowerFlags capabilities, unsigned long stateNumber, IOService *whatDevice) override;
    virtual IOReturn powerStateDidChangeTo(IOPMPowerFlags capabilities, unsigned long stateNumber, IOService *whatDevice) override;
    
public:
    virtual void free (void) override;
    virtual bool start (IOService* provider) override;
    virtual void stop (IOService* provider) override;
    virtual IOReturn setPowerState(unsigned long powerStateOrdinal, IOService *whatDevice) override;
    
    IOReturn myReadDataFromDevice();
};

#endif /* IOKitTest_hpp */


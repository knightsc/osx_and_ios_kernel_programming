#ifndef MYETHERNETDRIVER_H
#define MYETHERNETDRIVER_H

#include <IOKit/IOTimerEventSource.h>

#include <IOKit/network/IOEthernetController.h>
#include <IOKit/network/IOEthernetInterface.h>

#include "MyEthernetHwAbstraction.h"

#define MyEthernetDriver com_osxkernel_MyEthernetDriver

class MyEthernetDriver : public IOEthernetController
{
    friend class MyEthernetHwAbstraction;
    
    OSDeclareDefaultStructors(MyEthernetDriver);
    
public:
    
    virtual bool init(OSDictionary *properties) APPLE_KEXT_OVERRIDE;
    virtual void free() APPLE_KEXT_OVERRIDE;
    
    virtual bool start(IOService *provider) APPLE_KEXT_OVERRIDE;
    virtual void stop(IOService *provider) APPLE_KEXT_OVERRIDE;
    
    
    virtual bool configureInterface(IONetworkInterface *netif) APPLE_KEXT_OVERRIDE;

    virtual IOReturn enable(IONetworkInterface *netif) APPLE_KEXT_OVERRIDE;
    virtual IOReturn disable(IONetworkInterface *netif) APPLE_KEXT_OVERRIDE;
    
    virtual IOReturn getHardwareAddress(IOEthernetAddress *addrP) APPLE_KEXT_OVERRIDE;
    
    // Allow our driver's Mac address to be set
    virtual IOReturn setHardwareAddress(const IOEthernetAddress *addrP) APPLE_KEXT_OVERRIDE;
    
    virtual UInt32 outputPacket(mbuf_t m, void *param) APPLE_KEXT_OVERRIDE;
    
    
    virtual IOReturn setPromiscuousMode(bool active) APPLE_KEXT_OVERRIDE { return kIOReturnSuccess; }
    virtual IOReturn setMulticastMode(bool active) APPLE_KEXT_OVERRIDE { return kIOReturnSuccess; }
    
    bool createMediumDict();
    
private:
    
    static void  interruptOccured(OSObject *owner, IOTimerEventSource *sender);
    
    IOTimerEventSource *fInterruptSource;  // Simulate HW rx interrupt
    IOEthernetInterface *fNetworkInterface;
    OSDictionary *fMediumDict;
    IOWorkLoop *fWorkLoop;
    
    IONetworkStats *fNetworkStats;
    IOEthernetStats *fEthernetStats;
    
    MyEthernetHwAbstraction *fHWAbstraction; // Low-level hardware acecss.
};

#endif /* MYETHERNETDRIVER_H */

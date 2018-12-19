#ifndef MyEthernetDriver_h
#define MyEthernetDriver_h

#include <IOKit/assert.h>
#include <IOKit/IOTimerEventSource.h>
#include <IOKit/IOBufferMemoryDescriptor.h>
#include <IOKit/network/IOEthernetController.h>
#include <IOKit/network/IOEthernetInterface.h>
#include <IOKit/network/IOGatedOutputQueue.h>
#include <IOKit/network/IOMbufMemoryCursor.h>
#include <IOKit/network/IONetworkMedium.h>
#include <IOKit/IOUserClient.h>

#include "MyEthernetHwAbstraction.hpp"

class com_osxkernel_MyEthernetDriver : public IOEthernetController
{
    friend class com_osxkernel_MyEthernetHwAbstraction;
    
    OSDeclareDefaultStructors(com_osxkernel_MyEthernetDriver);
public:
    virtual bool init(OSDictionary *properties) override;
    virtual bool start(IOService *provider) override;
    virtual void stop(IOService *provider) override;
    virtual void free() override;
    
    virtual bool configureInterface(IONetworkInterface *netif) override;
    
    virtual IOReturn enable(IONetworkInterface *netif) override;
    virtual IOReturn disable(IONetworkInterface *netif) override;
    
    virtual IOReturn getHardwareAddress(IOEthernetAddress *addrP) override;
    
    // Allow our driver's Mac address to be set
    virtual IOReturn setHardwareAddress(const IOEthernetAddress *addrP) override;
    
    virtual UInt32 outputPacket(mbuf_t m, void *param) override;
    
    
    virtual IOReturn setPromiscuousMode(bool active) override { return kIOReturnSuccess; }
    virtual IOReturn setMulticastMode(bool active) override { return kIOReturnSuccess; }
    
    bool createMediumDict();
    
private:
    
    static void  interruptOccured(OSObject* owner, IOTimerEventSource* sender);
    
    
    IOTimerEventSource*     fInterruptSource;  // Simulate HW rx interrupt
    IOEthernetInterface*    fNetworkInterface;
    OSDictionary*           fMediumDict;
    IOWorkLoop*             fWorkLoop;
    
    IONetworkStats*         fNetworkStats;
    IOEthernetStats*        fEthernetStats;
    
    com_osxkernel_MyEthernetHwAbstraction* fHWAbstraction; // Low-level hardware acecss.
};

#endif

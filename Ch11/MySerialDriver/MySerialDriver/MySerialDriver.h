#ifndef MYSERIALDRIVER_H
#define MYSERIALDRIVER_H

#include <IOKit/serial/IOSerialDriverSync.h>

#define MySerialDriver com_osxkernel_MySerialDriver

class MySerialDriver : public IOSerialDriverSync
{
    OSDeclareDefaultStructors(MySerialDriver);

public:
    IOUSBHostInterface *fInterface;
    IOWorkLoop *fWorkLoop;
    IOCommandGate *fCommandGate;

    UInt32 m_currentState;
    UInt32 m_watchStateMask;
    
    // IOKit methods:
    
    virtual IOService *probe(IOService *provider, SInt32 *score) APPLE_KEXT_OVERRIDE;
    virtual bool start(IOService *provider) APPLE_KEXT_OVERRIDE;
    virtual void stop(IOService *provider) APPLE_KEXT_OVERRIDE;
    
    // IOSerialDriverSync Abstract Method Implementation
    
    virtual IOReturn acquirePort(bool sleep, void *refCon) APPLE_KEXT_OVERRIDE;
    virtual IOReturn releasePort(void *refCon) APPLE_KEXT_OVERRIDE;
    virtual UInt32 getState(void *refCon) APPLE_KEXT_OVERRIDE;
    virtual IOReturn setState(UInt32 state, UInt32 mask, void *refCon) APPLE_KEXT_OVERRIDE;
    virtual IOReturn watchState(UInt32 *state, UInt32 mask, void *refCon) APPLE_KEXT_OVERRIDE;
    virtual UInt32 nextEvent(void *refCon) APPLE_KEXT_OVERRIDE;
    virtual IOReturn executeEvent(UInt32 event, UInt32 data, void *refCon) APPLE_KEXT_OVERRIDE;
    virtual IOReturn requestEvent(UInt32 event, UInt32 *data, void *refCon) APPLE_KEXT_OVERRIDE;
    virtual IOReturn enqueueEvent(UInt32 event, UInt32 data, bool sleep, void *refCon) APPLE_KEXT_OVERRIDE;
    virtual IOReturn dequeueEvent(UInt32 *event, UInt32 *data, bool sleep, void *refCon) APPLE_KEXT_OVERRIDE;
    virtual IOReturn enqueueData(UInt8 *buffer, UInt32 size, UInt32 *count, bool sleep, void *refCon) APPLE_KEXT_OVERRIDE;
    virtual IOReturn dequeueData(UInt8 *buffer, UInt32 size, UInt32 *count, UInt32 min, void *refCon) APPLE_KEXT_OVERRIDE;
    
    // Static stubs for IOCommandGate::runAction
    static IOReturn acquirePortAction(OSObject *owner, void *arg0, void *, void *, void *);
    static IOReturn releasePortAction(OSObject *owner, void *, void *, void *, void *);
    static IOReturn getStateAction(OSObject *owner, void *, void *, void *, void *);
    static IOReturn setStateAction(OSObject *owner, void *arg0, void *arg1, void *, void *);
    static IOReturn watchStateAction(OSObject *owner, void *arg0, void *arg1, void *, void *);

    // Gated methods called by the Static stubs
    virtual IOReturn acquirePortGated(bool sleep, void *refCon);
    virtual IOReturn releasePortGated(void *refCon);
    virtual UInt32 getStateGated(void);
    virtual IOReturn setStateGated(UInt32 *pState, UInt32 *pMask);
    virtual IOReturn watchStateGated(UInt32 *state, UInt32 *pMask);

    // MySerialDriver methods
    bool createSerialStream(void);
};

#endif /* MYSERIALDRIVER_H */

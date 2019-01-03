#ifndef RAMDISKSTORAGEDEVICE_H
#define RAMDISKSTORAGEDEVICE_H

#include <IOKit/storage/IOBlockStorageDevice.h>

class com_osxkernel_driver_RAMDisk;

#define RAMDiskStorageDevice com_osxkernel_driver_RAMDiskStorageDevice

class RAMDiskStorageDevice : public IOBlockStorageDevice
{
    OSDeclareDefaultStructors(RAMDiskStorageDevice)
    
private:
    
    UInt64 m_blockCount;
    com_osxkernel_driver_RAMDisk *m_provider;
    
public:
    
    virtual bool init(UInt64 diskSize, OSDictionary *properties = 0);
    
    virtual bool attach(IOService *provider) APPLE_KEXT_OVERRIDE;
    virtual void detach(IOService *provider) APPLE_KEXT_OVERRIDE;
    
    virtual IOReturn doEjectMedia(void) APPLE_KEXT_OVERRIDE;
    virtual IOReturn doFormatMedia(UInt64 byteCapacity) APPLE_KEXT_OVERRIDE;
    virtual UInt32 doGetFormatCapacities(UInt64 *capacities, UInt32 capacitiesMaxCount) const APPLE_KEXT_OVERRIDE;
    virtual IOReturn doLockUnlockMedia(bool doLock) APPLE_KEXT_OVERRIDE;
    virtual IOReturn  doSynchronizeCache(void) APPLE_KEXT_OVERRIDE;
    virtual char *getVendorString(void) APPLE_KEXT_OVERRIDE;
    virtual char *getProductString(void) APPLE_KEXT_OVERRIDE;
    virtual char *getRevisionString(void) APPLE_KEXT_OVERRIDE;
    virtual char *getAdditionalDeviceInfoString(void) APPLE_KEXT_OVERRIDE;
    virtual IOReturn reportBlockSize(UInt64 *blockSize) APPLE_KEXT_OVERRIDE;
    virtual IOReturn reportEjectability(bool *isEjectable) APPLE_KEXT_OVERRIDE;
    virtual IOReturn reportLockability(bool *isLockable) APPLE_KEXT_OVERRIDE;
    virtual IOReturn reportMaxValidBlock(UInt64 *maxBlock) APPLE_KEXT_OVERRIDE;
    virtual IOReturn reportMediaState(bool *mediaPresent,bool *changedState) APPLE_KEXT_OVERRIDE;
    virtual IOReturn reportPollRequirements(bool *pollRequired, bool *pollIsExpensive) APPLE_KEXT_OVERRIDE;
    virtual IOReturn reportRemovability(bool *isRemovable) APPLE_KEXT_OVERRIDE;
    virtual IOReturn reportWriteProtection(bool *isWriteProtected) APPLE_KEXT_OVERRIDE;
    virtual IOReturn getWriteCacheState(bool *enabled) APPLE_KEXT_OVERRIDE;
    virtual IOReturn setWriteCacheState(bool enabled) APPLE_KEXT_OVERRIDE;
    virtual IOReturn doAsyncReadWrite(IOMemoryDescriptor *buffer, UInt64 block, UInt64 nblks, IOStorageAttributes *attributes, IOStorageCompletion *completion) APPLE_KEXT_OVERRIDE;
};

#endif /* RAMDISKSTORAGEDEVICE_H */

#include "RAMDiskStorageDevice.h"
#include "RAMDisk.h"

#define super IOBlockStorageDevice

OSDefineMetaClassAndStructors(RAMDiskStorageDevice, IOBlockStorageDevice)

#define kDiskBlockSize 512

bool RAMDiskStorageDevice::init(UInt64 diskSize, OSDictionary *properties)
{
    if (!super::init(properties)) {
        return false;
    }
    
    m_blockCount = diskSize / kDiskBlockSize;
    
    return true;
}

bool RAMDiskStorageDevice::attach(IOService *provider)
{
    if (!super::attach(provider)) {
        return false;
    }
    
    m_provider = OSDynamicCast(RAMDisk, provider);
    if (!m_provider) {
        return false;
    }
    
    return true;
}

void RAMDiskStorageDevice::detach(IOService *provider)
{
    if (m_provider == provider) {
        m_provider = NULL;
    }
    
    super::detach(provider);
}


IOReturn RAMDiskStorageDevice::doEjectMedia(void)
{
    return kIOReturnSuccess;
}

IOReturn RAMDiskStorageDevice::doFormatMedia(UInt64 byteCapacity)
{
    return kIOReturnSuccess;
}

UInt32 RAMDiskStorageDevice::doGetFormatCapacities(UInt64 *capacities, UInt32 capacitiesMaxCount) const
{
    // Ensure that the array is sufficient to hold all our formats (we require 1 element).
    if (capacities && (capacitiesMaxCount < 1)) {
        return 0;               // Error, return an array size of 0.
    }
    
    // The caller may provide a NULL array if it wishes to query the number of formats that we support.
    if (capacities) {
        capacities[0] = m_blockCount * kDiskBlockSize;
    }
    return 1;
}

IOReturn RAMDiskStorageDevice::doLockUnlockMedia(bool doLock)
{
    return kIOReturnUnsupported;
}

IOReturn RAMDiskStorageDevice::doSynchronizeCache(void)
{
    return kIOReturnSuccess;
}

char *RAMDiskStorageDevice::getVendorString(void)
{
    return (char *)"osxkernel.com";
}

char *RAMDiskStorageDevice::getProductString(void)
{
    return (char*)"RAM Disk";
}

char *RAMDiskStorageDevice::getRevisionString(void)
{
    return (char*)"1.0";
}

char *RAMDiskStorageDevice::getAdditionalDeviceInfoString(void)
{
    return NULL;
}

IOReturn RAMDiskStorageDevice::reportBlockSize(UInt64 *blockSize)
{
    *blockSize = kDiskBlockSize;
    return kIOReturnSuccess;
}

IOReturn RAMDiskStorageDevice::reportEjectability(bool *isEjectable)
{
    *isEjectable = false;
    return kIOReturnSuccess;
}

IOReturn RAMDiskStorageDevice::reportLockability(bool *isLockable)
{
    *isLockable = false;
    return kIOReturnSuccess;
}

IOReturn RAMDiskStorageDevice::reportMaxValidBlock(UInt64 *maxBlock)
{
    *maxBlock = m_blockCount - 1;
    return kIOReturnSuccess;
}

IOReturn RAMDiskStorageDevice::reportMediaState(bool *mediaPresent, bool *changedState)
{
    *mediaPresent = true;
    *changedState = false;
    
    return kIOReturnSuccess;
}

IOReturn RAMDiskStorageDevice::reportPollRequirements(bool *pollRequired, bool *pollIsExpensive)
{
    *pollRequired = false;
    *pollIsExpensive = false;
    return kIOReturnSuccess;
}

IOReturn RAMDiskStorageDevice::reportRemovability(bool *isRemovable)
{
    *isRemovable = true;
    return kIOReturnSuccess;
}

IOReturn RAMDiskStorageDevice::reportWriteProtection(bool *isWriteProtected)
{
    *isWriteProtected = false;
    return kIOReturnSuccess;
}

IOReturn RAMDiskStorageDevice::getWriteCacheState(bool *enabled)
{
    return kIOReturnUnsupported;
}

IOReturn RAMDiskStorageDevice::setWriteCacheState(bool enabled)
{
    return kIOReturnUnsupported;
}

IOReturn RAMDiskStorageDevice::doAsyncReadWrite(IOMemoryDescriptor *buffer, UInt64 block, UInt64 nblks, IOStorageAttributes *attributes, IOStorageCompletion *completion)
{
    IODirection direction;
    IOByteCount actualByteCount;
    
    // Return errors for incoming I/O if we have been terminated
    if (isInactive()) {
        return kIOReturnNotAttached;
    }
    
    direction = buffer->getDirection();
    if ((direction != kIODirectionIn) && (direction != kIODirectionOut)) {
        return kIOReturnBadArgument;
    }
    
    if ((block + nblks) > m_blockCount) {
        return kIOReturnBadArgument;
    }
    
    if (direction == kIODirectionIn) {
        actualByteCount = m_provider->performRead(buffer, (block * kDiskBlockSize), (nblks * kDiskBlockSize));
    }
    else {
        actualByteCount = m_provider->performWrite(buffer, (block * kDiskBlockSize), (nblks * kDiskBlockSize));
    }
    
    (completion->action)(completion->target, completion->parameter, kIOReturnSuccess, actualByteCount);
    
    return kIOReturnSuccess;
}

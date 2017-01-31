#include "MyFirstPCIDriver.hpp"

#define super IOService

OSDefineMetaClassAndStructors(com_osxkernel_MyFirstPCIDriver, IOService);

bool com_osxkernel_MyFirstPCIDriver::start(IOService *provider)
{
    IOLog("%s::start\n", getName());
    
    if (!super::start(provider)) {
        return false;
    }
    
    fPCIDevice = OSDynamicCast(IOPCIDevice, provider);
    if (!fPCIDevice) {
        return false;
    }
    
    fPCIDevice->setMemoryEnable(true);
    
    UInt16 vendorID = fPCIDevice->configRead16(kIOPCIConfigVendorID);
    UInt16 deviceID = fPCIDevice->configRead16(kIOPCIConfigDeviceID);
    
    IOLog("vendor ID = 0x%04x device ID = 0x%04x\n", vendorID, deviceID);
    
    IOByteCount offset = 0;
    if (fPCIDevice->extendedFindPCICapability(kIOPCIPCIExpressCapability, &offset)) {
        // See http://home.mit.bme.hu/~feher/MSC_RA/External_Bus/pci_express_10.pdf for
        // definition of different PCIe capability registers
        UInt16 linkStatusRegister = fPCIDevice->extendedConfigRead16(offset + 0x12);
        IOLog("link status register = 0x%04x\n", linkStatusRegister);
    }
    
    for (UInt32 i = 0; i < fPCIDevice->getDeviceMemoryCount(); i++) {
        IODeviceMemory* memoryDesc = fPCIDevice->getDeviceMemoryWithIndex(i);
        if (!memoryDesc) {
            continue;
        }
        IOLog("region%u: length=%llu bytes\n", i, memoryDesc->getLength());
    }
    
    registerService();
    
    return true;
}

void com_osxkernel_MyFirstPCIDriver::stop(IOService *provider)
{
    IOLog("%s::stop\n", getName());
    super::stop(provider);
}

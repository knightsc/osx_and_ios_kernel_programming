#include <IOKit/IOLib.h>
#include <IOKit/pci/IOPCIDevice.h>

class com_osxkernel_MyFirstPCIDriver : public IOService
{
    OSDeclareDefaultStructors(com_osxkernel_MyFirstPCIDriver);
    
private:
    IOPCIDevice* fPCIDevice;
    
public:
    virtual bool start(IOService* provider) override;
    virtual void stop(IOService* provider) override;
};

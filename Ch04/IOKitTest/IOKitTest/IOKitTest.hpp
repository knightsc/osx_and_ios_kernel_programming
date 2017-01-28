#include <IOKit/IOService.h>

#ifndef IOKitTest_hpp
#define IOKitTest_hpp

class com_osxkernel_driver_IOKitTest : public IOService
{
    OSDeclareDefaultStructors(com_osxkernel_driver_IOKitTest)
    
public:
    virtual bool init (OSDictionary* dictionary = NULL) override;
    virtual void free (void) override;

    virtual IOService* probe (IOService* provider, SInt32* score) override;
    virtual bool start (IOService* provider) override;
    virtual void stop (IOService* provider) override;
};

#endif /* IOKitTest_hpp */


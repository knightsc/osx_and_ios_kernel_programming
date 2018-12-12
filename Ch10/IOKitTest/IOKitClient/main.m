#include <Foundation/Foundation.h>
#include "TestDriverInterface.h"

int main(int argc, const char * argv[]) {
    @autoreleasepool {
        CFDictionaryRef matchingDict = NULL;
        io_service_t service = 0;
        
        matchingDict = IOServiceMatching("com_osxkernel_driver_IOKitTest");
        
        service = IOServiceGetMatchingService(kIOMasterPortDefault, matchingDict);
        
        if (!service) {
            return -1;
        }
        
        task_port_t owningTask = mach_task_self();
        uint32_t type = 0;
        io_connect_t driverConnection;
        kern_return_t kr;
        
        kr = IOServiceOpen(service, owningTask, type, &driverConnection);
        if (kr != KERN_SUCCESS) {
            return -1;
        }
        
        IOConnectCallMethod(driverConnection, kTestUserClientReadData, NULL, 0, NULL, 0, NULL, NULL, NULL, NULL);
        
        IOServiceClose(service);
        
        IOObjectRelease(service);
    }
    return 0;
}

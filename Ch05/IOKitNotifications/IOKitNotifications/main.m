#include <Foundation/Foundation.h>
#include <IOKit/IOKitLib.h>
#include <IOKit/IOMessage.h>

// Structure to describe a driver instance.
typedef struct {
    io_service_t    service;
    io_object_t     notification;
} MyDriverData;

// Notification port used for both device arrival and driver state changes.
IONotificationPortRef gNotificationPort = NULL;

void DeviceNotification(void *refCon, io_service_t service, natural_t messageType,
                        void *messageArgument)
{
    MyDriverData *myDriverData = (MyDriverData *)refCon;
    kern_return_t kr;
    
    // Only handle driver termination notifications.
    if (messageType == kIOMessageServiceIsTerminated) {
        // Print the name of the removed device.
        io_name_t name;
        IORegistryEntryGetName(service, name);
        
        printf("Device removed: %s\n", name);
        
        // Remove the driver state change notification.
        kr = IOObjectRelease(myDriverData->notification);
        
        // Release our reference to the driver object.
        IOObjectRelease(myDriverData->service);
        
        // Release our structure that holds the driver connection.
        free(myDriverData);
    }
}

void DeviceAdded(void *refCon, io_iterator_t iterator)
{
    io_service_t service = 0;
    
    // Iterate over all matching objects.
    while ((service = IOIteratorNext(iterator)) != 0) {
        CFStringRef className;
        io_name_t name;
        
        // List all IOUSBDevice objects, ignoring objects that subclass IOUSBDevice.
        className = IOObjectCopyClass(service);
        if (CFEqual(className, CFSTR("IOUSBDevice"))) {
            IORegistryEntryGetName(service, name);
            printf("Found device with name: %s\n", name);
            
            // Read properties
            CFTypeRef vendorName;
            vendorName = IORegistryEntryCreateCFProperty(service, CFSTR("USB Vendor Name"), kCFAllocatorDefault, 0);
            CFShow(vendorName);
            
            MyDriverData *myDriverData;
            kern_return_t kr;
            
            // Allocate a structure to hold the driver instance.
            myDriverData = (MyDriverData *)malloc(sizeof(MyDriverData));
            
            // Save the io_service_t for this driver instance.
            myDriverData->service = service;
            
            // Install a callback to receive notification of driver state changes.
            kr = IOServiceAddInterestNotification(gNotificationPort,
                                                  service,
                                                  kIOGeneralInterest,
                                                  DeviceNotification,
                                                  myDriverData,
                                                  &myDriverData->notification);
        }
        CFRelease(className);
        IOObjectRelease(service);
    }
}

int main(int argc, const char * argv[])
{
    @autoreleasepool {
        CFDictionaryRef matchingDict = NULL;
        io_iterator_t iter = 0;
        CFRunLoopSourceRef runLoopSource;
        kern_return_t kr;
        
        // Create a matching dictionary that will find any USB device
        matchingDict = IOServiceMatching("IOUSBDevice");
        gNotificationPort = IONotificationPortCreate(kIOMasterPortDefault);
        runLoopSource = IONotificationPortGetRunLoopSource(gNotificationPort);
        
        CFRunLoopAddSource(CFRunLoopGetCurrent(), runLoopSource, kCFRunLoopDefaultMode);
        
        kr = IOServiceAddMatchingNotification(gNotificationPort, kIOFirstMatchNotification,
                                              matchingDict, DeviceAdded, NULL, &iter);
        
        DeviceAdded(NULL, iter);
        
        CFRunLoopRun();
        
        IONotificationPortDestroy(gNotificationPort);
        
        // Release the iterator
        IOObjectRelease(iter);
    }
    return 0;
}

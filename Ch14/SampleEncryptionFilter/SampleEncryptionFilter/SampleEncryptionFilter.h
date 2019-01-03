#ifndef SAMPLEENCRYPTIONFILTER_H
#define SAMPLEENCRYPTIONFILTER_H

#include <IOKit/storage/IOFilterScheme.h>

#define SampleEncryptionFilter com_osxkernel_driver_SampleEncryptionFilter

class SampleEncryptionFilter : public IOFilterScheme
{
    OSDeclareDefaultStructors(SampleEncryptionFilter)
    
protected:
    
    IOMedia *m_encryptedMedia;
    IOMedia *m_childMedia;
    
    struct ReadCompletionParams {
        IOStorageCompletion completion;
        IOMemoryDescriptor *buffer;
    };
    
    IOMedia *instantiateMediaObject();
    static void readCompleted(void *target, void *parameter, IOReturn status, UInt64 actualByteCount);
    
public:
    
    virtual bool start(IOService *provider) APPLE_KEXT_OVERRIDE;
    virtual void stop(IOService *provider) APPLE_KEXT_OVERRIDE;
    virtual void free(void) APPLE_KEXT_OVERRIDE;
    
    virtual void read(IOService *client, UInt64 byteStart, IOMemoryDescriptor *buffer, IOStorageAttributes *attributes, IOStorageCompletion *completion) APPLE_KEXT_OVERRIDE;
    virtual void write(IOService *client, UInt64 byteStart, IOMemoryDescriptor *buffer, IOStorageAttributes* attributes, IOStorageCompletion *completion) APPLE_KEXT_OVERRIDE;
    
    static IOReturn decryptBuffer(IOMemoryDescriptor *buffer, UInt64 actualByteCount);
    static IOMemoryDescriptor *encryptBuffer(IOMemoryDescriptor *buffer);
};

#endif /* SAMPLEENCRYPTIONFILTER_H */

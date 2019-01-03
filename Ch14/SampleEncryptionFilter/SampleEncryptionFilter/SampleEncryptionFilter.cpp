#include <IOKit/IOLib.h>
#include <IOKit/IOBufferMemoryDescriptor.h>

#include "SampleEncryptionFilter.h"

#define super IOFilterScheme

OSDefineMetaClassAndStructors(SampleEncryptionFilter, IOFilterScheme)

bool SampleEncryptionFilter::start(IOService *provider)
{
    if (!super::start(provider)) {
        return false;
    }
    
    m_encryptedMedia = OSDynamicCast(IOMedia, provider);
    if (!m_encryptedMedia) {
        return false;
    }
    
    m_childMedia = instantiateMediaObject();
    if (!m_childMedia) {
        return false;
    }
    
    if (!m_childMedia->attach(this)) {
        return false;
    }
    m_childMedia->registerService();
    
    return true;
}

void SampleEncryptionFilter::stop(IOService *provider)
{
    super::stop(provider);
}

void SampleEncryptionFilter::free(void)
{
    if (m_childMedia) {
        m_childMedia->release();
    }
    
    super::free();
}

IOMedia *SampleEncryptionFilter::instantiateMediaObject()
{
    IOMedia *newMedia;
    
    newMedia = new IOMedia;
    if (newMedia) {
        if (newMedia->init(0, m_encryptedMedia->getSize(), m_encryptedMedia->getPreferredBlockSize(), m_encryptedMedia->getAttributes(), false, m_encryptedMedia->isWritable(), "Apple_HFS")) {
            // Set a location value (the partition number) for this partition
            newMedia->setLocation("1");
        }
        else {
            newMedia->release();
            newMedia = NULL;
        }
    }
    
    return newMedia;
}


void SampleEncryptionFilter::read(IOService *client, UInt64 byteStart, IOMemoryDescriptor *buffer, IOStorageAttributes *attributes, IOStorageCompletion *completion)
{
    ReadCompletionParams *context;
    IOStorageCompletion newCompletion;
    
    context = (ReadCompletionParams *)IOMalloc(sizeof(ReadCompletionParams));
    if (!context) {
        complete(completion, kIOReturnNoMemory);
        return;
    }
    
    context->completion = *completion;
    context->buffer = buffer;
    context->buffer->retain();
    
    newCompletion.target = this;
    newCompletion.action = readCompleted;
    newCompletion.parameter = context;
    
    m_encryptedMedia->read(client, byteStart, buffer, attributes, &newCompletion);
}

void SampleEncryptionFilter::readCompleted(void *target, void *parameter, IOReturn status, UInt64 actualByteCount)
{
    ReadCompletionParams *context = (ReadCompletionParams *)parameter;
    
    // Decrypt the data read from disk.
    if (status == kIOReturnSuccess) {
        status = decryptBuffer(context->buffer, actualByteCount);
    }
    
    // If  either the read from disk or the decryption operation failed, set the actualByteCount value to 0.
    if (status != kIOReturnSuccess) {
        actualByteCount = 0;
    }
    
    // Call the original caller’s completion function.
    complete(&context->completion, status, actualByteCount);
    
    context->buffer->release();
    IOFree(context, sizeof(ReadCompletionParams));
}

IOReturn SampleEncryptionFilter::decryptBuffer(IOMemoryDescriptor *buffer, UInt64 actualByteCount)
{
    bool didPrepare = false;
    IOMemoryMap *map = NULL;
    uint32_t *nextWord;
    IOReturn status;
    
    status = buffer->prepare(buffer->getDirection());
    if (status != kIOReturnSuccess) {
        goto bail;
    }
    didPrepare = true;
    map = buffer->map();
    if (!map) {
        status = kIOReturnError;
        goto bail;
    }
    
    // Decrypt the data
    UInt64 remainingWords;
    remainingWords = actualByteCount / sizeof(uint32_t);
    nextWord = (uint32_t *)map->getVirtualAddress();
    while (remainingWords--)
    {
        *nextWord ^= 0xFFFFFFFF;
        nextWord++;
    }
    
    // Fall-through on success
bail:
    
    if (map != NULL)
        map->release();
    if (didPrepare == true)
        buffer->complete();
    
    return status;
}


void SampleEncryptionFilter::write(IOService *client, UInt64 byteStart, IOMemoryDescriptor *buffer, IOStorageAttributes *attributes, IOStorageCompletion *completion)
{
    IOMemoryDescriptor *newDesc;
    
    newDesc = encryptBuffer(buffer);
    if (!newDesc) {
        complete(completion, kIOReturnNoMemory);
        return;
    }
    
    m_encryptedMedia->write(client, byteStart, newDesc, attributes, completion);
    
    newDesc->release();
}

IOMemoryDescriptor *SampleEncryptionFilter::encryptBuffer(IOMemoryDescriptor *buffer)
{
    IOBufferMemoryDescriptor *newDesc;
    
    // Allocate a buffer to hold the encrypted contents
    newDesc = IOBufferMemoryDescriptor::withCapacity(buffer->getLength(), buffer->getDirection());
    if (newDesc) {
        uint32_t *nextWord;
        UInt64 remainingWords;
        
        nextWord = (uint32_t *)newDesc->getBytesNoCopy();
        
        // Read the source buffer into the new memory descriptor
        buffer->readBytes(0, nextWord, buffer->getLength());
        
        // Encrypt the buffer
        remainingWords = buffer->getLength() / sizeof(uint32_t);
        while (remainingWords--) {
            *nextWord ^= 0xFFFFFFFF;
            nextWord++;
        }
    }
    
    return newDesc;
}

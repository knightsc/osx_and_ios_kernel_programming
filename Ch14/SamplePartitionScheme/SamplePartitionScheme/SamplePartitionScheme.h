#ifndef SAMPLEPARTITIONSCHEME_H
#define SAMPLEPARTITIONSCHEME_H

#include <IOKit/storage/IOPartitionScheme.h>

#define SamplePartitionIdentifier "Sample Partition Scheme"

#define PartitionScheme com_osxkernel_driver_PartitionScheme

struct SamplePartitionEntry {
    UInt64 blockStart;
    UInt64 blockCount;
};

struct SamplePartitionTable {
    char partitionIdentifier[24];
    UInt64 partitionCount;
    SamplePartitionEntry partitionEntries[30];
};


class PartitionScheme : public IOPartitionScheme
{
    OSDeclareDefaultStructors(PartitionScheme)
    
protected:
    
    OSSet *m_partitions;
    
    virtual OSSet *scan(SInt32 *score);
    virtual IOMedia *instantiateMediaObject(SamplePartitionEntry *sampleEntry, unsigned index);
    bool isPartitionCorrupt(SamplePartitionEntry *sampleEntry) { return false; }
    
public:
    
    virtual IOService *probe(IOService *provider, SInt32 *score) APPLE_KEXT_OVERRIDE;
    virtual bool start(IOService *provider) APPLE_KEXT_OVERRIDE;
    virtual void stop(IOService *provider) APPLE_KEXT_OVERRIDE;
    virtual void free(void) APPLE_KEXT_OVERRIDE;
    virtual IOReturn requestProbe(IOOptionBits options) APPLE_KEXT_OVERRIDE;
};

#endif /* SAMPLEPARTITIONSCHEME_H */

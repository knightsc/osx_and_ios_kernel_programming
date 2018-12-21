#ifndef MYAUDIOENGINE_H
#define MYAUDIOENGINE_H

#include <IOKit/audio/IOAudioEngine.h>

#include "MyAudioDevice.h"

#define MyAudioEngine com_osxkernel_MyAudioEngine

class MyAudioEngine : public IOAudioEngine
{
    OSDeclareDefaultStructors(MyAudioEngine)
    
public:
    virtual void free() APPLE_KEXT_OVERRIDE;
    
    virtual bool initHardware(IOService *provider) APPLE_KEXT_OVERRIDE;
    virtual void stop(IOService *provider) APPLE_KEXT_OVERRIDE;
    
    virtual IOAudioStream *createNewAudioStream(IOAudioStreamDirection direction, void *sampleBuffer, UInt32 sampleBufferSize);
    
    virtual IOReturn performAudioEngineStart() APPLE_KEXT_OVERRIDE;
    virtual IOReturn performAudioEngineStop() APPLE_KEXT_OVERRIDE;
    
    virtual UInt32 getCurrentSampleFrame() APPLE_KEXT_OVERRIDE;
    
    virtual IOReturn performFormatChange(IOAudioStream *audioStream, const IOAudioStreamFormat *newFormat, const IOAudioSampleRate *newSampleRate) APPLE_KEXT_OVERRIDE;
    
    virtual IOReturn clipOutputSamples(const void *mixBuf, void *sampleBuf, UInt32 firstSampleFrame, UInt32 numSampleFrames, const IOAudioStreamFormat *streamFormat, IOAudioStream *audioStream) APPLE_KEXT_OVERRIDE ;
    virtual IOReturn convertInputSamples(const void *sampleBuf, void *destBuf, UInt32 firstSampleFrame, UInt32 numSampleFrames, const IOAudioStreamFormat *streamFormat, IOAudioStream *audioStream) APPLE_KEXT_OVERRIDE;
    
private:
    IOTimerEventSource *fAudioInterruptSource;
    SInt16 *fOutputBuffer;
    SInt16 *fInputBuffer;
    
    static void interruptOccured(OSObject *owner, IOTimerEventSource *sender);
    void handleAudioInterrupt();
    
    UInt32 fInterruptCount;
    
    SInt64 fNextTimeout;
    
};

#endif /* MYAUDIOENGINE_H */

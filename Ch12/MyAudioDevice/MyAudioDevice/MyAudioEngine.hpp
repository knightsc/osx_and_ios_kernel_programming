#ifndef _MYAUDIOENGINE_H_
#define _MYAUDIOENGINE_H_

#include <IOKit/audio/IOAudioEngine.h>

#include "MyAudioDevice.hpp"

#define MyAudioEngine com_osxkernel_MyAudioEngine

class MyAudioEngine : public IOAudioEngine
{
    OSDeclareDefaultStructors(MyAudioEngine)
    
public:
    virtual void free() override;
    
    virtual bool initHardware(IOService *provider) override;
    virtual void stop(IOService *provider) override;
    
    virtual IOAudioStream *createNewAudioStream(IOAudioStreamDirection direction, void *sampleBuffer, UInt32 sampleBufferSize);
    
    virtual IOReturn performAudioEngineStart() override;
    virtual IOReturn performAudioEngineStop() override;
    
    virtual UInt32 getCurrentSampleFrame() override;
    
    virtual IOReturn performFormatChange(IOAudioStream *audioStream, const IOAudioStreamFormat *newFormat, const IOAudioSampleRate *newSampleRate) override;
    
    virtual IOReturn clipOutputSamples(const void *mixBuf, void *sampleBuf, UInt32 firstSampleFrame, UInt32 numSampleFrames, const IOAudioStreamFormat *streamFormat, IOAudioStream *audioStream) override;
    virtual IOReturn convertInputSamples(const void *sampleBuf, void *destBuf, UInt32 firstSampleFrame, UInt32 numSampleFrames, const IOAudioStreamFormat *streamFormat, IOAudioStream *audioStream) override;
    
private:
    IOTimerEventSource*     fAudioInterruptSource;
    SInt16                  *fOutputBuffer;
    SInt16                    *fInputBuffer;
    
    static void             interruptOccured(OSObject* owner, IOTimerEventSource* sender);
    void                    handleAudioInterrupt();
    
    UInt32                  fInterruptCount;
    
    SInt64                  fNextTimeout;
    
};

#endif

#include <IOKit/IOLib.h>

#include "IOKitTest.h"

#define super IOService

OSDefineMetaClassAndStructors(IOKitTest, IOService)

bool IOKitTest::init(OSDictionary *dict)
{
    bool res = super::init(dict);
    IOLog("IOKitTest::init\n");
    return res;
}

void IOKitTest::free(void)
{
    IOLog("IOKitTest::free\n");
    super::free();
}

IOService *IOKitTest::probe(IOService *provider, SInt32 *score)
{
    IOService *res = super::probe(provider, score);
    IOLog("IOKitTest::probe\n");
    return res;
}

bool IOKitTest::start(IOService *provider)
{
    bool res = super::start(provider);
    IOLog("IOKitTest::start\n");
    
    super::registerService();
    
    return res;
}

void IOKitTest::stop(IOService *provider)
{
    IOLog("IOKitTest::stop\n");
    super::stop(provider);
}

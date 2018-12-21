#include <libkern/libkern.h>
#include <mach/mach_types.h>

kern_return_t HelloWorld_start(kmod_info_t *ki, void *d);
kern_return_t HelloWorld_stop(kmod_info_t *ki, void *d);

kern_return_t HelloWorld_start(kmod_info_t *ki, void *d)
{
    printf("Hello world\n");
    return KERN_SUCCESS;
}

kern_return_t HelloWorld_stop(kmod_info_t *ki, void *d)
{
    printf("Goodbye world\n");
    return KERN_SUCCESS;
}

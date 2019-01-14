#include <libkern/libkern.h>
#include <mach/mach_types.h>

extern int Shared_Exported(void);

kern_return_t
Driver_start(kmod_info_t *ki, void *d)
{
    int i = Shared_Exported();
    printf("Driver_start: %d\n", i);
    return KERN_SUCCESS;
}

kern_return_t
Driver_stop(kmod_info_t *ki, void *d)
{
    return KERN_SUCCESS;
}

#include <libkern/libkern.h>
#include <mach/mach_types.h>

int
Shared_Exported()
{
    return 5;
}

static int
Shared_NotExported()
{
    return 10;
}

kern_return_t
Shared_start(kmod_info_t *ki, void *d)
{
    int i = Shared_NotExported();
    printf("Shared_start: %d\n", i);
    return KERN_SUCCESS;
}

kern_return_t
Shared_stop(kmod_info_t *ki, void *d)
{
    return KERN_SUCCESS;
}

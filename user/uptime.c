#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(void)
{
    int ticks = uptime();   
    printf("%d ticks\n", ticks);
    exit(0);
}

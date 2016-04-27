#include "systeminit.h"
#include "debug.h"

int main(void)
{
    p_drUser_SystemInit();
    while(1)
    {
        p_dr_UartDebug();
    }
}


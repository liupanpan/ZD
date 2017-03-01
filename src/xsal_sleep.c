#include "xsal.h"

#include <unistd.h>

void SAL_Sleep(uint32_t milliseconds)
{
   (void)usleep(milliseconds*1000u);
}

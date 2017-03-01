#include "xsal.h"

#include "unistd.h"

void SAL_Delay(uint32_t microseconds)
{
   (void)usleep(microseconds);
}


#include "xsal.h"


/* -----------------------------
 *  Public function definition
 * -----------------------------*/


SAL_Int_T SAL_Delta_Time(SAL_Clock_T start_time, SAL_Clock_T end_time)
{
      return (SAL_Int_T) (end_time - start_time);
}


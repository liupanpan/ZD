#include "xsal.h"


/* -----------------------------
 *  Public function definition
 * -----------------------------*/


SAL_Int_T SAL_Elapsed_Time(SAL_Clock_T timestamp)
{
   return (SAL_Int_T)(SAL_Clock() - timestamp);
}

#ifndef XSAL_I_ASSERT_H
#define XSAL_I_ASSERT_H

#include <stdlib.h>
#include <assert.h>

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#define SAL_PRE(cond)                  \
   do                                  \
   {                                   \
      if (!(cond))                     \
      {                                \
         printf("SAL_PRE(" #cond ")"); \
         abort();                      \
      }                                \
   } while(0)

#define SAL_PRE_FAILED()                  \
   do                                     \
   {                                      \
      printf("SAL_PRE_FAILED()");        \
      abort();                            \
   } while(0)




#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* XSAL_I_ASSERT_H */


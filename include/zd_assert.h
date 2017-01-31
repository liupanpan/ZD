#ifndef XSAL_ASSERT_H
#define XSAL_ASSERT_H

#include <stdlib.h>
#include <assert.h>

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#define ZD_CON(cond)                  	\
   do                                  	\
   {                                   	\
      if (!(cond))                     	\
      {                                	\
        abort();                      	\
      }                                	\
   } while(0)

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* XSAL_ASSERT_H */


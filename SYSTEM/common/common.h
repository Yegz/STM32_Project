#ifndef COMMON__H_
#define COMMON__H_
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

/***************************************************************************************
* Define
****************************************************************************************/
/*use this to clear armcc complain*/
#define UNUSED(var)                     (var)=(var)
#define MEM_B(x)                        (*((byte *)(x)))             // Get Address X one byte
#define MEM_W(x)                        (*((word *)(x)))             // Get Address X one word
#define ARR_SIZE(a)                     (sizeof((a))/sizeof((a[0]))) // Get array size
#define B_PTR(var)                      ((byte *)(void *) &(var))    // Get the variable byte address 
#define W_PTR(var)                      ((word *)(void *) &(var))    // Get the variable word address 
#define WORD_LO(x)                      ((byte)((word)(x) & 0xFF))   // Get one word low byte data
#define WORD_HI(x)                      ((byte)((word)(x) >> 8))     // Get one word high byte data
#define MALLOC_FREE(ptr)                {free(ptr); ptr = NULL;}     // Free the malloc memory

#ifndef NULL
#define NULL ((void *)0)
#endif

/***************************************************************************************
* Data
***************************************************************************************/
typedef enum
{
    FALSE = 0,
    TRUE,
}Bool_t;

/***************************************************************************************
* Function prototypes
****************************************************************************************/

#endif


#ifndef __KEY_H
#define __KEY_H	 
#include "debug.h"

/***************************************************************************************
* Define
****************************************************************************************/
#define KEY0                            PAin(13)  
#define KEY1                            PAin(15)
#define KEY2                            PAin(0)

#define INDEPENDENT_KEY0                0x00000001
#define INDEPENDENT_KEY1                0x00000002
#define INDEPENDENT_KEY2                0x00000004
#define INDEPENDENT_KEY3                0x00000008
#define INDEPENDENT_KEY4                0x00000010
#define INDEPENDENT_KEY5                0x00000020
#define INDEPENDENT_KEY6                0x00000040
#define INDEPENDENT_KEY7                0x00000080
#define INDEPENDENT_KEY8                0x00000100
#define INDEPENDENT_KEY9                0x00000200
#define INDEPENDENT_KEY10               0x00000400
#define INDEPENDENT_KEY11               0x00000800
#define INDEPENDENT_KEY12               0x00001000
#define INDEPENDENT_KEY13               0x00002000
#define INDEPENDENT_KEY14               0x00004000
// 15~31 4*4 Matrix Keyboard Key Map
#define MATRIX_KEYBOARD_KEYX1Y1         0x00008000
#define MATRIX_KEYBOARD_KEYX1Y2         0x00010000
#define MATRIX_KEYBOARD_KEYX1Y3         0x00020000
#define MATRIX_KEYBOARD_KEYX1Y4         0x00040000
#define MATRIX_KEYBOARD_KEYX2Y1         0x00080000
#define MATRIX_KEYBOARD_KEYX2Y2         0x00100000
#define MATRIX_KEYBOARD_KEYX2Y3         0x00200000
#define MATRIX_KEYBOARD_KEYX2Y4         0x00400000
#define MATRIX_KEYBOARD_KEYX3Y1         0x00800000
#define MATRIX_KEYBOARD_KEYX3Y2         0x01000000
#define MATRIX_KEYBOARD_KEYX3Y3         0x02000000
#define MATRIX_KEYBOARD_KEYX3Y4         0x04000000
#define MATRIX_KEYBOARD_KEYX4Y1         0x08000000
#define MATRIX_KEYBOARD_KEYX4Y2         0x10000000
#define MATRIX_KEYBOARD_KEYX4Y3         0x20000000
#define MATRIX_KEYBOARD_KEYX4Y4         0x40000000

#define KEY_MAX_NUM                     16
#define KEY_BUF_SIZE                    10
#define KEY_LONG_PRESS_ENABLE           1
#define KEY_SHORT_SHIFT                 MATRIX_KEYBOARD_KEYX4Y4

#if KEY_LONG_PRESS_ENABLE > 0
#define KEY_PRESS_DELAY_CYCLE           100    // Press 2s  -> long press.
#define KEY_PRESS_TIMER_CYCLE           50
#define KEY_LONG_SHIFT                  MATRIX_KEYBOARD_KEYX4Y4
#endif

#if (KEY_MAX_NUM > 15)                  
#define KEY_TYPE                        u32
#define KEY_LONG_PRESS_MASK             0x80000000
#define INVALID_KEY                     0xFFFFFFFF
#elif (KEY_MAX_NUM > 7)
#define KEY_TYPE                        u16
#define KEY_LONG_PRESS_MASK             0x8000
#define INVALID_KEY                     0xFFFF
#else
#define KEY_TYPE                        u8
#define KEY_LONG_PRESS_MASK             0x80
#define INVALID_KEY                     0xFF
#endif

// KEY Config
#define MATRIX_KEYBOARD_ROW1_LOW		GPIO_ResetBits(GPIOA, GPIO_Pin_1)
#define MATRIX_KEYBOARD_ROW1_HIGH		GPIO_SetBits(GPIOA, GPIO_Pin_1)
#define MATRIX_KEYBOARD_ROW2_LOW		GPIO_ResetBits(GPIOA, GPIO_Pin_2)
#define MATRIX_KEYBOARD_ROW2_HIGH		GPIO_SetBits(GPIOA, GPIO_Pin_2)
#define MATRIX_KEYBOARD_ROW3_LOW		GPIO_ResetBits(GPIOA, GPIO_Pin_3)
#define MATRIX_KEYBOARD_ROW3_HIGH		GPIO_SetBits(GPIOA, GPIO_Pin_3)
#define MATRIX_KEYBOARD_ROW4_LOW		GPIO_ResetBits(GPIOA, GPIO_Pin_4)
#define MATRIX_KEYBOARD_ROW4_HIGH		GPIO_SetBits(GPIOA, GPIO_Pin_4)

#define MATRIX_KEYBOARD_GET_COLUMN1		GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_6)
#define MATRIX_KEYBOARD_GET_COLUMN2		GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_7)
#define MATRIX_KEYBOARD_GET_COLUMN3		GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_8)
#define MATRIX_KEYBOARD_GET_COLUMN4		GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_9)


/***************************************************************************************
* Data
***************************************************************************************/
typedef enum 
{  
    BUTTON_KEY0 = 0,
    BUTTON_KEY1,
    BUTTON_WAKEUP,
    BUTTON_ESC,
    BUTTON_ENTER,
    BUTTON_UP,
    BUTTON_DOWN,
    BUTTON_LEFT,
    BUTTON_RIGHT,
    MATRIX_KEYBOARD_KEY1,
    MATRIX_KEYBOARD_KEY2,
    MATRIX_KEYBOARD_KEY3,
    MATRIX_KEYBOARD_KEY4,
    MATRIX_KEYBOARD_KEY5,
    MATRIX_KEYBOARD_KEY6,
    MATRIX_KEYBOARD_KEY7,
    MATRIX_KEYBOARD_KEY8,
    MATRIX_KEYBOARD_KEY9,
    MATRIX_KEYBOARD_KEY10,
    MATRIX_KEYBOARD_KEY11,
    MATRIX_KEYBOARD_KEY12,
    MATRIX_KEYBOARD_KEY13,
    MATRIX_KEYBOARD_KEY14,
    MATRIX_KEYBOARD_KEY15,
    MATRIX_KEYBOARD_KEY16,
    BUTTON_KEY0_LONG_PRESS,
    BUTTON_KEY1_LONG_PRESS,
    BUTTON_WAKEUP_LONG_PRESS,
    BUTTON_ESC_LONG_PRESS,
    BUTTON_ENTER_LONG_PRESS,
    BUTTON_UP_LONG_PRESS,
    BUTTON_DOWN_LONG_PRESS,
    BUTTON_LEFT_LONG_PRESS,
    BUTTON_RIGHT_LONG_PRESS,
    MATRIX_KEYBOARD_KEY1_LONG_PRESS,
    MATRIX_KEYBOARD_KEY2_LONG_PRESS,
    MATRIX_KEYBOARD_KEY3_LONG_PRESS,
    MATRIX_KEYBOARD_KEY4_LONG_PRESS,
    MATRIX_KEYBOARD_KEY5_LONG_PRESS,
    MATRIX_KEYBOARD_KEY6_LONG_PRESS,
    MATRIX_KEYBOARD_KEY7_LONG_PRESS,
    MATRIX_KEYBOARD_KEY8_LONG_PRESS,
    MATRIX_KEYBOARD_KEY9_LONG_PRESS,
    MATRIX_KEYBOARD_KEY10_LONG_PRESS,
    MATRIX_KEYBOARD_KEY11_LONG_PRESS,
    MATRIX_KEYBOARD_KEY12_LONG_PRESS,
    MATRIX_KEYBOARD_KEY13_LONG_PRESS,
    MATRIX_KEYBOARD_KEY14_LONG_PRESS,
    MATRIX_KEYBOARD_KEY15_LONG_PRESS,
    MATRIX_KEYBOARD_KEY16_LONG_PRESS,
    BUTTON_INVALID,
}Button_TypeDef;

typedef struct  
{   
    GPIO_TypeDef*        BUTTON_PORT ;
    uint16_t             BUTTON_PIN ;
    GPIOMode_TypeDef     BUTTON_MODE;
}Button_PinModeDef; 

/***************************************************************************************
* Function prototypes
****************************************************************************************/	 
void            p_dr_KeyInit(void);
void            p_dr_MatrixKeyBoardInit(void);
u8              p_dr_KeyScan(void); 			
void            p_dr_MatrixKeyBoardScan(void);
static KEY_TYPE p_dr_MatrixKeyBoardRead(void);
static KEY_TYPE p_dr_KeyBufOut(void);
static void     p_dr_KeyBufIn(KEY_TYPE KeyVal);
Button_TypeDef  p_dr_GetKeyVal(void);
bool            p_dr_IsMatrixKeyPress(void);
void            p_dr_MatrixKeyFlush(void);
void            p_dr_KeyDebug(void);
#endif

/* == HISTORY =========================================================
 *
 * Name     Date                Ver     Action
 * --------------------------------------------------------------------
 * Carols   15-May-2016         Git     Add Matrix KeyBoard Functions
 *
 **/

#include "stm32f10x.h"
#include "key.h"
#include "sys.h"
#include "delay.h"
#include "timer.h"

/**************************************************************************************
* Data
**************************************************************************************/
u8 G_u8_KeyFifoInIndex = 0;
u8 G_u8_KeyFifoOutIndex = 0;
u8 G_u8_FifoKeyNum = 0;
KEY_TYPE G_KeyFifo[KEY_BUF_SIZE];
static KEY_TYPE G_s_PreKeyScanVal = 0;
static KEY_TYPE G_s_PreReadKeyVal = 0;
static KEY_TYPE G_s_KeyMask = 0;
#if KEY_LONG_PRESS_ENABLE
static u8 G_s_u8_KeyPressTmr = KEY_PRESS_DELAY_CYCLE;
#endif

/**************************************************************************************
* Function Implementation
**************************************************************************************/
/*************************************************************************************************************
Function Name       £ºp_dr_KeyInit()
Input               £ºNULL
Output              £ºNULL
Function Description£ºKey init.
*************************************************************************************************************/
void p_dr_KeyInit(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_13|GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}

/*************************************************************************************************************
Function Name       £ºp_dr_MatrixKeyBoardInit()
Input               £ºNULL
Output              £ºNULL
Function Description£ºMatrix keyboard init.
*************************************************************************************************************/
void p_dr_MatrixKeyBoardInit(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA| RCC_APB2Periph_GPIOC, ENABLE);

    // ROW
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4;
    GPIO_ResetBits(GPIOA, GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4);
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // COLUMN
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    // Init Timer3, Scan One Timer Per 20ms
    p_dr_TimerxInit(199, 7200);
}

/*************************************************************************************************************
Function Name       £ºp_dr_KeyScan()
Input               £ºNULL
Output              £ºNULL
Function Description£ºKey scan.
*************************************************************************************************************/
u8 p_dr_KeyScan(void)
{
    static u8 key_up=1;

    GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE);
    if(key_up&&(KEY0==0 || KEY1==0 || KEY2==1))
    {
        delay_ms(10);                                           // remove shake
        key_up=0;
        if(KEY0==0)
        {
            GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
            p_dr_KeyBufIn(INDEPENDENT_KEY0);
            return BUTTON_KEY0;
        }
        else if(KEY1==0)
        {
            GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
            p_dr_KeyBufIn(INDEPENDENT_KEY1);
            return BUTTON_KEY1;
        }
        else if(KEY2==1)
        {
            GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
            p_dr_KeyBufIn(INDEPENDENT_KEY2);
            return BUTTON_WAKEUP;
        }
    }
    else if(KEY0==1 && KEY1==1 && KEY2==0)
        key_up=1;

    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);

    return 0;
}

/*************************************************************************************************************
Function Name       £ºp_dr_MatrixKeyBoardScan()
Input               £ºNULL
Output              £ºNULL
Function Description£ºMatrix keyboard scan.
                      1: Support:
                        Short press;
                        Long press;
                      2: Get KeyVal:
                        CurKeyVal & G_s_PreKeyVal: Level trigger;
                        CurKeyVal ^ G_s_PreKeyVal: Edge trigger;
                        CurKeyVal & (CurKeyVal ^ G_s_PreKeyVal) or (~G_s_PreKeyVal) & CurKeyVal: Rising edge trigger;
                        G_s_PreKeyVal & (NowKey ^ PreKey) or PreKey & (~NowKey): Falling edge trigger.
                      3: Filtering algorithm
                        PreScanKey & NowScanKey: Level trigger
                        PreReadKey & (PreScanKey ^ NowScanKey): Sampling keep
                        NowReadKey = 1) | 2): Level trigger with sampling keep.
                      4: Note
                        Key scan should between 20 ~ 50ms, Then it support filtering.
*************************************************************************************************************/
void p_dr_MatrixKeyBoardScan(void)
{
    KEY_TYPE CurKeyScanVal = 0;
    KEY_TYPE CurReadKeyVal = 0;
    KEY_TYPE KeyRelease = 0;

    CurKeyScanVal = p_dr_MatrixKeyBoardRead();
    CurReadKeyVal = (G_s_PreKeyScanVal&CurKeyScanVal) | G_s_PreReadKeyVal & (G_s_PreKeyScanVal^CurKeyScanVal);
    KeyRelease    = G_s_PreReadKeyVal & (CurReadKeyVal ^ G_s_PreReadKeyVal);
#if KEY_LONG_PRESS_ENABLE
    if((CurReadKeyVal == G_s_PreReadKeyVal) && CurReadKeyVal)
    {
        G_s_u8_KeyPressTmr--;
        if(!G_s_u8_KeyPressTmr)
        {
            p_dr_KeyBufIn(CurReadKeyVal | KEY_LONG_PRESS_MASK);
            G_s_u8_KeyPressTmr = KEY_PRESS_TIMER_CYCLE;
            G_s_KeyMask = CurReadKeyVal;
        }
    }
    else
    {
        G_s_u8_KeyPressTmr = KEY_PRESS_DELAY_CYCLE;
    }
#endif

    if(KeyRelease)
    {
        if(KeyRelease & (~G_s_KeyMask))
        {
            p_dr_KeyBufIn(KeyRelease);
        }
        else
        {
            G_s_KeyMask = 0;
        }
    }

    G_s_PreKeyScanVal = CurKeyScanVal;
    G_s_PreReadKeyVal = CurReadKeyVal;
}

/*************************************************************************************************************
Function Name       £ºp_dr_MatrixKeyBoardRead()
Input               £ºNULL
Output              £ºNULL
Function Description£ºMatrix keyboard scan.
*************************************************************************************************************/
static KEY_TYPE p_dr_MatrixKeyBoardRead(void)
{
    KEY_TYPE KeyVal = 0;

    MATRIX_KEYBOARD_ROW1_HIGH;
    if(MATRIX_KEYBOARD_GET_COLUMN1)
    {
        KeyVal |= MATRIX_KEYBOARD_KEYX1Y1;
    }
    if(MATRIX_KEYBOARD_GET_COLUMN2)
    {
        KeyVal |= MATRIX_KEYBOARD_KEYX1Y2;
    }
    if(MATRIX_KEYBOARD_GET_COLUMN3)
    {
        KeyVal |= MATRIX_KEYBOARD_KEYX1Y3;
    }
    if(MATRIX_KEYBOARD_GET_COLUMN4)
    {
        KeyVal |= MATRIX_KEYBOARD_KEYX1Y4;
    }
    MATRIX_KEYBOARD_ROW1_LOW;

    MATRIX_KEYBOARD_ROW2_HIGH;
    if(MATRIX_KEYBOARD_GET_COLUMN1)
    {
        KeyVal |= MATRIX_KEYBOARD_KEYX2Y1;
    }
    if(MATRIX_KEYBOARD_GET_COLUMN2)
    {
        KeyVal |= MATRIX_KEYBOARD_KEYX2Y2;
    }
    if(MATRIX_KEYBOARD_GET_COLUMN3)
    {
        KeyVal |= MATRIX_KEYBOARD_KEYX2Y3;
    }
    if(MATRIX_KEYBOARD_GET_COLUMN4)
    {
        KeyVal |= MATRIX_KEYBOARD_KEYX2Y4;
    }
    MATRIX_KEYBOARD_ROW2_LOW;

    MATRIX_KEYBOARD_ROW3_HIGH;
    if(MATRIX_KEYBOARD_GET_COLUMN1)
    {
        KeyVal |= MATRIX_KEYBOARD_KEYX3Y1;
    }
    if(MATRIX_KEYBOARD_GET_COLUMN2)
    {
        KeyVal |= MATRIX_KEYBOARD_KEYX3Y2;
    }
    if(MATRIX_KEYBOARD_GET_COLUMN3)
    {
        KeyVal |= MATRIX_KEYBOARD_KEYX3Y3;
    }
    if(MATRIX_KEYBOARD_GET_COLUMN4)
    {
        KeyVal |= MATRIX_KEYBOARD_KEYX3Y4;
    }
    MATRIX_KEYBOARD_ROW3_LOW;

    MATRIX_KEYBOARD_ROW4_HIGH;
    if(MATRIX_KEYBOARD_GET_COLUMN1)
    {
        KeyVal |= MATRIX_KEYBOARD_KEYX4Y1;
    }
    if(MATRIX_KEYBOARD_GET_COLUMN2)
    {
        KeyVal |= MATRIX_KEYBOARD_KEYX4Y2;
    }
    if(MATRIX_KEYBOARD_GET_COLUMN3)
    {
        KeyVal |= MATRIX_KEYBOARD_KEYX4Y3;
    }
    if(MATRIX_KEYBOARD_GET_COLUMN4)
    {
        KeyVal |= MATRIX_KEYBOARD_KEYX4Y4;
    }
    MATRIX_KEYBOARD_ROW4_LOW;

    return KeyVal;
}

/*************************************************************************************************************
Function Name       £ºp_dr_MatrixKeyBoardBufOut()
Input               £ºNULL
Output              £ºReturn the key value, No pull down return 0xFFFF
Function Description£ºGet a key value from the FIFO, if no key pull down, It will return 0xFF.
*************************************************************************************************************/
static KEY_TYPE p_dr_KeyBufOut(void)
{
    KEY_TYPE KeyVal = INVALID_KEY;

    if(G_u8_FifoKeyNum > 0)
    {
        if(INVALID_KEY != G_KeyFifo[G_u8_KeyFifoOutIndex])
        {
            G_u8_FifoKeyNum--;
            KeyVal = G_KeyFifo[G_u8_KeyFifoOutIndex];
            G_KeyFifo[G_u8_KeyFifoOutIndex] = INVALID_KEY;
        }

        G_u8_KeyFifoOutIndex++;
        if (G_u8_KeyFifoOutIndex >= KEY_BUF_SIZE)
        {
            G_u8_KeyFifoOutIndex = 0;
        }
    }

    return KeyVal;
}

/*************************************************************************************************************
Function Name       £ºp_dr_MatrixKeyBoardBufIn()
Input               £ºKey value
Output              £ºNULL
Function Description£ºSave the key value to FIFO buf.
*************************************************************************************************************/
static void p_dr_KeyBufIn(KEY_TYPE KeyVal)
{
    if(G_u8_FifoKeyNum >= KEY_BUF_SIZE)
    {
        p_dr_KeyBufOut();
    }

    G_u8_FifoKeyNum++;
    if(G_u8_FifoKeyNum > KEY_BUF_SIZE)
        G_u8_FifoKeyNum = KEY_BUF_SIZE;

    G_KeyFifo[G_u8_KeyFifoInIndex++] = KeyVal;
    if (G_u8_KeyFifoInIndex >= KEY_BUF_SIZE)
    {
        if(INVALID_KEY == G_KeyFifo[G_u8_KeyFifoOutIndex])
            G_u8_KeyFifoInIndex = KEY_BUF_SIZE;
        else
            G_u8_KeyFifoInIndex = 0;
    }
}

/*************************************************************************************************************
Function Name       £ºp_dr_GetKeyVal()
Input               £ºNULL
Output              £ºNULL
Function Description£ºGet the key value.
*************************************************************************************************************/
Button_TypeDef p_dr_GetKeyVal(void)
{
    Button_TypeDef KeyVal;
    KEY_TYPE ReadKeyVal;

    ReadKeyVal = p_dr_KeyBufOut();

    if(ReadKeyVal & KEY_LONG_PRESS_MASK)
    {
        switch(ReadKeyVal & (~KEY_LONG_PRESS_MASK))
        {
            case INDEPENDENT_KEY0:
                KeyVal = BUTTON_KEY0_LONG_PRESS;
                break;
            case INDEPENDENT_KEY1:
                KeyVal = BUTTON_KEY1_LONG_PRESS;
                break;
            case INDEPENDENT_KEY2:
                KeyVal = BUTTON_WAKEUP_LONG_PRESS;
                break;
            case INDEPENDENT_KEY3:
                KeyVal = BUTTON_ESC_LONG_PRESS;
                break;
            case INDEPENDENT_KEY4:
                KeyVal = BUTTON_ENTER_LONG_PRESS;
                break;
            case INDEPENDENT_KEY5:
                KeyVal = BUTTON_UP_LONG_PRESS;
                break;
            case INDEPENDENT_KEY6:
                KeyVal = BUTTON_DOWN_LONG_PRESS;
                break;
            case INDEPENDENT_KEY7:
                KeyVal = BUTTON_LEFT_LONG_PRESS;
                break;
            case INDEPENDENT_KEY8:
                KeyVal = BUTTON_RIGHT_LONG_PRESS;
                break;
            case MATRIX_KEYBOARD_KEYX1Y1:
                KeyVal = MATRIX_KEYBOARD_KEY1_LONG_PRESS;
                break;
            case MATRIX_KEYBOARD_KEYX1Y2:
                KeyVal = MATRIX_KEYBOARD_KEY2_LONG_PRESS;
                break;
            case MATRIX_KEYBOARD_KEYX1Y3:
                KeyVal = MATRIX_KEYBOARD_KEY3_LONG_PRESS;
                break;
            case MATRIX_KEYBOARD_KEYX1Y4:
                KeyVal = MATRIX_KEYBOARD_KEY4_LONG_PRESS;
                break;
            case MATRIX_KEYBOARD_KEYX2Y1:
                KeyVal = MATRIX_KEYBOARD_KEY5_LONG_PRESS;
                break;
            case MATRIX_KEYBOARD_KEYX2Y2:
                KeyVal = MATRIX_KEYBOARD_KEY6_LONG_PRESS;
                break;
            case MATRIX_KEYBOARD_KEYX2Y3:
                KeyVal = MATRIX_KEYBOARD_KEY7_LONG_PRESS;
                break;
            case MATRIX_KEYBOARD_KEYX2Y4:
                KeyVal = MATRIX_KEYBOARD_KEY8_LONG_PRESS;
                break;
            case MATRIX_KEYBOARD_KEYX3Y1:
                KeyVal = MATRIX_KEYBOARD_KEY9_LONG_PRESS;
                break;
            case MATRIX_KEYBOARD_KEYX3Y2:
                KeyVal = MATRIX_KEYBOARD_KEY10_LONG_PRESS;
                break;
            case MATRIX_KEYBOARD_KEYX3Y3:
                KeyVal = MATRIX_KEYBOARD_KEY11_LONG_PRESS;
                break;
            case MATRIX_KEYBOARD_KEYX3Y4:
                KeyVal = MATRIX_KEYBOARD_KEY12_LONG_PRESS;
                break;
            case MATRIX_KEYBOARD_KEYX4Y1:
                KeyVal = MATRIX_KEYBOARD_KEY13_LONG_PRESS;
                break;
            case MATRIX_KEYBOARD_KEYX4Y2:
                KeyVal = MATRIX_KEYBOARD_KEY14_LONG_PRESS;
                break;
            case MATRIX_KEYBOARD_KEYX4Y3:
                KeyVal = MATRIX_KEYBOARD_KEY15_LONG_PRESS;
                break;
            case MATRIX_KEYBOARD_KEYX4Y4:
                KeyVal = MATRIX_KEYBOARD_KEY16_LONG_PRESS;
                break;
            case INVALID_KEY:
                KeyVal = BUTTON_INVALID;
                break;
            default:
                KeyVal = BUTTON_INVALID;
                break;
        }
    }
    else
    {
        switch(ReadKeyVal)
        {
            case INDEPENDENT_KEY0:
                KeyVal = BUTTON_KEY0;
                break;
            case INDEPENDENT_KEY1:
                KeyVal = BUTTON_KEY1;
                break;
            case INDEPENDENT_KEY2:
                KeyVal = BUTTON_WAKEUP;
                break;
            case INDEPENDENT_KEY3:
                KeyVal = BUTTON_ESC;
                break;
            case INDEPENDENT_KEY4:
                KeyVal = BUTTON_ENTER;
                break;
            case INDEPENDENT_KEY5:
                KeyVal = BUTTON_UP;
                break;
            case INDEPENDENT_KEY6:
                KeyVal = BUTTON_DOWN;
                break;
            case INDEPENDENT_KEY7:
                KeyVal = BUTTON_LEFT;
                break;
            case INDEPENDENT_KEY8:
                KeyVal = BUTTON_RIGHT;
                break;
            case MATRIX_KEYBOARD_KEYX1Y1:
                KeyVal = MATRIX_KEYBOARD_KEY1;
                break;
            case MATRIX_KEYBOARD_KEYX1Y2:
                KeyVal = MATRIX_KEYBOARD_KEY2;
                break;
            case MATRIX_KEYBOARD_KEYX1Y3:
                KeyVal = MATRIX_KEYBOARD_KEY3;
                break;
            case MATRIX_KEYBOARD_KEYX1Y4:
                KeyVal = MATRIX_KEYBOARD_KEY4;
                break;
            case MATRIX_KEYBOARD_KEYX2Y1:
                KeyVal = MATRIX_KEYBOARD_KEY5;
                break;
            case MATRIX_KEYBOARD_KEYX2Y2:
                KeyVal = MATRIX_KEYBOARD_KEY6;
                break;
            case MATRIX_KEYBOARD_KEYX2Y3:
                KeyVal = MATRIX_KEYBOARD_KEY7;
                break;
            case MATRIX_KEYBOARD_KEYX2Y4:
                KeyVal = MATRIX_KEYBOARD_KEY8;
                break;
            case MATRIX_KEYBOARD_KEYX3Y1:
                KeyVal = MATRIX_KEYBOARD_KEY9;
                break;
            case MATRIX_KEYBOARD_KEYX3Y2:
                KeyVal = MATRIX_KEYBOARD_KEY10;
                break;
            case MATRIX_KEYBOARD_KEYX3Y3:
                KeyVal = MATRIX_KEYBOARD_KEY11;
                break;
            case MATRIX_KEYBOARD_KEYX3Y4:
                KeyVal = MATRIX_KEYBOARD_KEY12;
                break;
            case MATRIX_KEYBOARD_KEYX4Y1:
                KeyVal = MATRIX_KEYBOARD_KEY13;
                break;
            case MATRIX_KEYBOARD_KEYX4Y2:
                KeyVal = MATRIX_KEYBOARD_KEY14;
                break;
            case MATRIX_KEYBOARD_KEYX4Y3:
                KeyVal = MATRIX_KEYBOARD_KEY15;
                break;
            case MATRIX_KEYBOARD_KEYX4Y4:
                KeyVal = MATRIX_KEYBOARD_KEY16;
                break;
            case INVALID_KEY:
                KeyVal = BUTTON_INVALID;
                break;
            default:
                KeyVal = BUTTON_INVALID;
                break;
        }
    }
    
    return KeyVal;
}

/*************************************************************************************************************
Function Name       £ºp_dr_IsMatrixKeyPress()
Input               £ºNULL
Output              £ºNULL
Function Description£ºWhether matrix keyboard have key press down.
*************************************************************************************************************/
bool p_dr_IsMatrixKeyPress(void)
{
    bool ret;

    ret = (bool)(G_u8_FifoKeyNum > 0)? TRUE:FALSE;

    return ret;
}

/*************************************************************************************************************
Function Name       £ºp_dr_MatrixKeyFlush()
Input               £ºNULL
Output              £ºNULL
Function Description£ºReset the matrix keyboard FIFO.
*************************************************************************************************************/
void p_dr_MatrixKeyFlush(void)
{
    while(p_dr_GetKeyVal());
}


void p_dr_KeyDebug(void)
{
    KEY_TYPE KeyVal;

    KeyVal = p_dr_GetKeyVal();
    switch(KeyVal)
    {
        case BUTTON_KEY0:
            printf("button0\r\n");
            break;
        case BUTTON_KEY1:
            printf("button1\r\n");
            break;
        case BUTTON_WAKEUP:
            printf("wake up\r\n");
            break;
        case BUTTON_ESC:
            printf("Esc\r\n");
            break;
        case BUTTON_ENTER:
            printf("Enter\r\n");
            break;
        case BUTTON_UP:
            printf("Up\r\n");
            break;
        case BUTTON_DOWN:
            printf("Down\r\n");
            break;
        case BUTTON_LEFT:
            printf("Left\r\n");
            break;
        case BUTTON_RIGHT:
            printf("Right\r\n");
            break;
        case MATRIX_KEYBOARD_KEY1:
            printf("key1\r\n");
            break;
        case MATRIX_KEYBOARD_KEY2:
            printf("key2\r\n");
            break;
        case MATRIX_KEYBOARD_KEY3:
            printf("key3\r\n");
            break;
        case MATRIX_KEYBOARD_KEY4:
            printf("key4\r\n");
            break;
        case MATRIX_KEYBOARD_KEY5:
            printf("key5\r\n");
            break;
        case MATRIX_KEYBOARD_KEY6:
            printf("key6\r\n");
            break;
        case MATRIX_KEYBOARD_KEY7:
            printf("key7\r\n");
            break;
        case MATRIX_KEYBOARD_KEY8:
            printf("key8\r\n");
            break;
        case MATRIX_KEYBOARD_KEY9:
            printf("key9\r\n");
            break;
        case MATRIX_KEYBOARD_KEY10:
            printf("key10\r\n");
            break;
        case MATRIX_KEYBOARD_KEY11:
            printf("key11\r\n");
            break;
        case MATRIX_KEYBOARD_KEY12:
            printf("key12\r\n");
            break;
        case MATRIX_KEYBOARD_KEY13:
            printf("key13\r\n");
            break;
        case MATRIX_KEYBOARD_KEY14:
            printf("key14\r\n");
            break;
        case MATRIX_KEYBOARD_KEY15:
            printf("key15\r\n");
            break;
        case MATRIX_KEYBOARD_KEY16:
            printf("key16\r\n");
            break;
        case BUTTON_KEY0_LONG_PRESS:
            printf("Long button0\r\n");
            break;
        case BUTTON_KEY1_LONG_PRESS:
            printf("Long button1\r\n");
            break;
        case BUTTON_WAKEUP_LONG_PRESS:
            printf("Long wake up\r\n");
            break;
        case BUTTON_ESC_LONG_PRESS:
            printf("Long Esc\r\n");
            break;
        case BUTTON_ENTER_LONG_PRESS:
            printf("Long Enter\r\n");
            break;
        case BUTTON_UP_LONG_PRESS:
            printf("Long Up\r\n");
            break;
        case BUTTON_DOWN_LONG_PRESS:
            printf("Long Down\r\n");
            break;
        case BUTTON_LEFT_LONG_PRESS:
            printf("Long Left\r\n");
            break;
        case BUTTON_RIGHT_LONG_PRESS:
            printf("Long Right\r\n");
            break;
        case MATRIX_KEYBOARD_KEY1_LONG_PRESS:
            printf("Long key1\r\n");
            break;
        case MATRIX_KEYBOARD_KEY2_LONG_PRESS:
            printf("Long key2\r\n");
            break;
        case MATRIX_KEYBOARD_KEY3_LONG_PRESS:
            printf("Long key3\r\n");
            break;
        case MATRIX_KEYBOARD_KEY4_LONG_PRESS:
            printf("Long key4\r\n");
            break;
        case MATRIX_KEYBOARD_KEY5_LONG_PRESS:
            printf("Long key5\r\n");
            break;
        case MATRIX_KEYBOARD_KEY6_LONG_PRESS:
            printf("Long key6\r\n");
            break;
        case MATRIX_KEYBOARD_KEY7_LONG_PRESS:
            printf("Long key7\r\n");
            break;
        case MATRIX_KEYBOARD_KEY8_LONG_PRESS:
            printf("Long key8\r\n");
            break;
        case MATRIX_KEYBOARD_KEY9_LONG_PRESS:
            printf("Long key9\r\n");
            break;
        case MATRIX_KEYBOARD_KEY10_LONG_PRESS:
            printf("Long key10\r\n");
            break;
        case MATRIX_KEYBOARD_KEY11_LONG_PRESS:
            printf("Long key11\r\n");
            break;
        case MATRIX_KEYBOARD_KEY12_LONG_PRESS:
            printf("Long key12\r\n");
            break;
        case MATRIX_KEYBOARD_KEY13_LONG_PRESS:
            printf("Long key13\r\n");
            break;
        case MATRIX_KEYBOARD_KEY14_LONG_PRESS:
            printf("Long key14\r\n");
            break;
        case MATRIX_KEYBOARD_KEY15_LONG_PRESS:
            printf("Long key15\r\n");
            break;
        case MATRIX_KEYBOARD_KEY16_LONG_PRESS:
            printf("Long key16\r\n");
            break;
        default:
            break;
    }
}


#include "Config.h"
#include "12864.h"
#include "Delay.h"
#include "Uart.h"
#include "Menu.h"
#include "Key.h"
#include "Game_Pong.h"
#include "Game_Snake.h"

#include "pic12864.h"

/*------------------------------ Menu level 1 -------------------------------*/

MENU_PRMT xdata Main_MenuParam;                  

MENU_ITEM code Main_MenuItem[] =                       
{
    {"1. Fun Games    ", Menu_Games     },
    {"2. Setting      ", Menu_Setting   },
    {"3. About System ", Menu_SystemInfo},               
};

MENU_INFO code Main_MenuInfo = 
{
    "-= MAIN MENU =- ",
    &Menu_SplashScreen,
    Main_MenuItem,
    sizeof(Main_MenuItem)/sizeof(Main_MenuItem[0]),
};

/*------------------------------ Menu level 2 -------------------------------*/

MENU_PRMT xdata Games_MenuParam;                   

MENU_ITEM code Games_MenuItem[] =                           
{
    {"1. Pong         ", PongGame_Main },//Menu_Null_Info },      
    {"2. Greedy Snake ", SnakeGame_Main},                 
    {"3. Tetris       ", Menu_Null_Info},                 
    {"4.  NULL        ", Menu_Null_Info},                   
    {"5.  NULL        ", Menu_Null_Info},                 
};

MENU_INFO code Games_MenuInfo = 
{
    "  -= GAMES =-  ",
    &Menu_Main,   
    Games_MenuItem,
    sizeof(Games_MenuItem)/sizeof(Games_MenuItem[0]),
};

MENU_PRMT xdata Setting_MenuParam;                          

MENU_ITEM code Setting_MenuItem[] =                        
{
    {"1. Brightness   ", Menu_Null_Info},                    
    {"2. Date & Time  ", Menu_Null_Info},                   
    {"3. Password     ", Menu_Null_Info},                   
    {"4. Language     ", Menu_Null_Info},                   
};

MENU_INFO code Setting_MenuInfo = 
{
    " -= SETTING =- ",
    &Menu_Main,
    Setting_MenuItem,
    sizeof(Setting_MenuItem)/sizeof(Setting_MenuItem[0]),
};

bit fKeyTrig = TRUE;                // ����������־
bit fClrDDRAM = TRUE;               // ��Ļ����(DDRAM)��־ - ˳��ˢ��menuName
bit fMenuParamInit = TRUE;          // �˵�������ʼ����־
bit fCursorInit = TRUE;             // ���λ�ó�ʼ����־

xdata void(*pfCurrentFunc)(void) = &Menu_Games;     // ָ��ǰ���еĲ˵������ĺ���ָ��
//xdata void(*pfCurrentFunc)(void) = &PongGame_Main;     // ָ��ǰ���еĲ˵������ĺ���ָ��
//xdata void(*pfCurrentFunc)(void) = &Menu_SplashScreen;     // ָ��ǰ���еĲ˵������ĺ���ָ��

static void Menu_ParamInit(MENU_PRMT xdata *param, u8 num, u8 page);
static void Menu_Display(MENU_ITEM code *menu, u8 page, u8 dispNum, u8 cursor);
static void Menu_Move(MENU_PRMT xdata *param, MENU_INFO code *menuInfo);
static void Menu_Process(MENU_INFO code *menuInfo, MENU_PRMT xdata *param);

/*****************************************************************************
 * Function      : Menu_ParamInit
 * Description   : Init Menu Param
 * Param         : MENU_PRMT xdata *param  
                   u8 num                  
                   u8 page                 
 * Returns       : None
 *
 * Record        :
 * 1.Date        : 2016/09/12
 *   Author      : Mr_Liang
 *   Modification: Created function
*****************************************************************************/
static void Menu_ParamInit(MENU_PRMT xdata *param, u8 num, u8 page)
{
    if (fCursorInit)
    {
        fCursorInit   = FALSE;
        param->Cursor = FALSE;                     // �������
    }
    param->PageNo   = FALSE;                     // ҳ����
    param->Index    = FALSE;                     // ��������
    param->DispNum  = num;                       // ҳ�����ʾ��Ŀ��
    param->MaxPage  = page;                      // ���ҳ��
}  

/*****************************************************************************
 * Function      : Menu_Display
 * Description   : Display tables
 * Param         : MENU_ITEM code *menuItem  
                   u8 page                
                   u8 dispNum             
                   u8 cursor              
 * Returns       : None
 *
 * Record        :
 * 1.Date        : 2016/09/12
 *   Author      : Mr_Liang
 *   Modification: Created function
*****************************************************************************/
static void Menu_Display(MENU_ITEM code *menuItem, u8 page, u8 dispNum, u8 cursor)
{
    u8 xdata i;

    LcdWrCmd(EN_EXT_INSTRUCTION);
    LcdClrGDRAM();

    for (i = 0; i < dispNum; ++i)
    {
        if (cursor == i) 
        {
            LcdPutStr(0, i+1, 1, menuItem[page+i].pszMenuName);
        }
        else 
        {
            LcdWrCmd(EN_BAS_INSTRUCTION);
            LcdPutStr(0, i+1, 0, menuItem[page+i].pszMenuName);
        }
    }

    LcdWrCmd(EXT_GRAPH_ON);
    LcdWrCmd(EN_BAS_INSTRUCTION);
}

/*****************************************************************************
 * Function      : Menu_Move
 * Description   : Menu Move Handling
 * Param         : MENU_PRMT xdata *param      Menu Parameter
                   MENU_ITEM code *menuItem  Menu Table
                   MENU_INFO code *menuInfo    Menu Info.
 * Returns       : None
 *
 * Record        :
 * 1.Date        : 2016/09/12
 *   Author      : Mr_Liang
 *   Modification: Created function
 *
 * 2.Date        : 2016/10/03
 *   Author      : Mr_Liang
 *   Modification: Version_2
*****************************************************************************/
static void Menu_Move(MENU_PRMT xdata *param, MENU_INFO code *menuInfo)
{
    switch (KeyValue)
    {
    case KV_UP:                   // ����
        KeyValue = KV_NULL;
        fKeyTrig = TRUE;

        if (param->Cursor != 0)                       // ��겻�ڶ���
        {
            param->Cursor--;                        // �������
        }
        else                        // ����ڶ���
        {
            if (param->PageNo != 0)                      // ҳ��û�е���С
            {
                param->PageNo--;                       // ���Ϸ�
            }
            else
            {
                param->Cursor = param->DispNum-1;                      // ��굽��
                param->PageNo = param->MaxPage-1;                  // ���ҳ
            }
        }
            break;

    case KV_DOWN:                   // ����
        KeyValue = KV_NULL;
        fKeyTrig = TRUE;

        if (param->Cursor < param->DispNum-1)                          // ���û�е���,�ƶ����
        {
            param->Cursor++;                        // ��������ƶ�
        }
        else                                                         // ��굽��
        {
            if (param->PageNo < param->MaxPage-1)                      // ҳ��û�е���,ҳ���ƶ�
            {
                param->PageNo++;                       // �·�һҳ
            }
            else                                                     // ҳ��͹�궼����,���ؿ�ʼҳ
            {
                param->Cursor = 0;
                param->PageNo = 0;
            }
        }
            break;

    case KV_LEFT:
        KeyValue = KV_NULL;

            break;

    case KV_RIGHT:
        KeyValue = KV_NULL;

            break;

    case KV_ENTER:                  // ȷ��
        KeyValue       = KV_NULL;
        fKeyTrig       = TRUE;
        fMenuParamInit = TRUE;
        fClrDDRAM      = TRUE;
        fCursorInit    = TRUE;

        param->Index = param->Cursor + param->PageNo;                   // ����ִ���������
//        menuItem[param->Index].CH_PF();                               // ִ����Ӧ��
        menuInfo->psItem[param->Index].pfChildFunc();
            break;

    case KV_ESC:                    // �˳�
        KeyValue       = KV_NULL;
        fKeyTrig       = TRUE;
        fMenuParamInit = TRUE;
        fClrDDRAM      = TRUE;

        menuInfo->pfFatherFunc();
            break;

    /*case KV_LF:                   // ��������
        param->Cursor = 0;
        param->PageNo = 0;
            break;

    case KV_RT:                   // �����ײ�
        param->Cursor = param->DispNum-1;                              // ��굽��
        param->PageNo = param->MaxPage-1;                          // ���ҳ
            break;*/

    default:
//        KeyValue = KV_NULL;
            break;
    }
                          // ����ִ������
}

/*****************************************************************************
 * Function      : Menu_Process
 * Description   : ����˵���
 * Param         : MENU_INFO code *menuInfo   
                   MENU_PRMT xdata *param     
 * Returns       : None
 *
 * Record        :
 * 1.Date        : 2016/08/26
 *   Author      : Mr_Liang
 *   Modification: Created function
 *
 * 2.Date        : 2016/10/03
 *   Author      : Mr_Liang
 *   Modification: Version_2
*****************************************************************************/
static void Menu_Process(MENU_INFO code *menuInfo, MENU_PRMT xdata *param)
{
    Menu_Move(param, menuInfo);
    
    if (fClrDDRAM) 
    {
        fClrDDRAM = FALSE;

        LcdWrCmd(EN_BAS_INSTRUCTION);
        LcdWrCmd(CLR_DDRAM_INIT_AC);
        LcdPutStr(0, 0, 0, menuInfo->pszTitle);
    }

    if (fMenuParamInit)                     // Init menu parameter 
    {                     
        fMenuParamInit = FALSE;
        
        Menu_ParamInit(param, PAGE_DISP_NUM, menuInfo->nItems - PAGE_DISP_NUM + 1);                          // ��ʾ������ҳ������
    }

    if (fKeyTrig)
    {
        fKeyTrig = FALSE;

        Menu_Display(menuInfo->psItem, param->PageNo, param->DispNum, param->Cursor);               // ��ʾ�˵���
    }
}

/*****************************************************************************
 * Function      : Menu_Main
 * Description   : Main Menu
 * Param         : void  
 * Returns       : None
 *
 * Record        :
 * 1.Date        : 2016/09/12
 *   Author      : Mr_Liang
 *   Modification: Created function
*****************************************************************************/
void Menu_Main(void)
{
    pfCurrentFunc = &Menu_Main;

    Menu_Process(&Main_MenuInfo, &Main_MenuParam);
}

/*****************************************************************************
 * Function      : Menu_Games
 * Description   : Games Menu
 * Param         : void  
 * Returns       : None
 *
 * Record        :
 * 1.Date        : 2016/09/12
 *   Author      : Mr_Liang
 *   Modification: Created function       
*****************************************************************************/
void Menu_Games(void)
{
    pfCurrentFunc = &Menu_Games;
//    KeyValue = KV_ENTER;

    Menu_Process(&Games_MenuInfo, &Games_MenuParam);
}

/*****************************************************************************
 * Function      : Menu_Setting
 * Description   : Setting
 * Param         : void  
 * Returns       : None
 *
 * Record        :
 * 1.Date        : 2016/09/12
 *   Author      : Mr_Liang
 *   Modification: Created function
*****************************************************************************/
void Menu_Setting(void)
{
    pfCurrentFunc = &Menu_Setting;

    Menu_Process(&Setting_MenuInfo, &Setting_MenuParam);
}

void SetMenuFlag(bit status) 
{
    fKeyTrig       = status;
    fMenuParamInit = status;
    fClrDDRAM      = status;
    fCursorInit    = status;
}

//static void Menu

/*****************************************************************************
 * Function      : Menu_SplashScreen
 * Description   : Start up Picture
 * Param         : void  
 * Returns       : None
 *
 * Record        :
 * 1.Date        : 2016/09/12
 *   Author      : Mr_Liang
 *   Modification: Created function
*****************************************************************************/
void Menu_SplashScreen(void)
{
    pfCurrentFunc = &Menu_SplashScreen;
        
    if (fKeyTrig) 
    {
        fKeyTrig = FALSE;
        SetMenuFlag(FALSE);

        LcdWrCmd(EN_BAS_INSTRUCTION);
        LcdWrCmd(CLR_DDRAM_INIT_AC);
        LcdWrCmd(EN_EXT_INSTRUCTION);
        LcdClrGDRAM();
        LcdDrawG_12864(zhizhang);
        UartSendDebugInfo("generate\r\n");
    }

    if (KeyValue == KV_ENTER) 
    {
        KeyValue = KV_NULL;

        SetMenuFlag(TRUE);
        
        Menu_Main();
    }
}

void Menu_SystemInfo(void)
{
    pfCurrentFunc = &Menu_SystemInfo;
    
    if (fKeyTrig) 
    {
        SetMenuFlag(FALSE);
        
        LcdWrCmd(CLR_DDRAM_INIT_AC);

        LcdPutStr(0, 0, 0, "Version: V4.0.0");
        LcdPutStr(0, 1, 0, "Compile Info:");
        LcdPutStr(1, 2, 0, __DATE__);
        LcdPutStr(1, 3, 0, __TIME__);
    }

    if (KeyValue == KV_ESC) 
    {
        KeyValue = KV_NULL;     // ����ȴ�ʱ���µļ�ֵ
        
        SetMenuFlag(TRUE);
        fMenuParamInit = FALSE;       // ��ֹ��ʼ���˵�����, ʹ���ͣ����ԭ��
        fCursorInit    = FALSE;

        Menu_Main();
    }

}

void Menu_Service(void(*func_ptr)())
{
    func_ptr();
}

/*****************************************************************************
 * Function      : Func_Null
 * Description   : NULL Function
 * Param         : void  
 * Returns       : None
 *
 * Record        :
 * 1.Date        : 2016/09/12
 *   Author      : Mr_Liang
 *   Modification: Created function
*****************************************************************************/
void Func_Null(void)
{
    ;
}

/*****************************************************************************
 * Function      : Menu_Null_Info
 * Description   : Display "In Development" on the screen.
 * Param         : void  
 * Returns       : None
 *
 * Record        :
 * 1.Date        : 2016/09/12
 *   Author      : Mr_Liang
 *   Modification: Created function
*****************************************************************************/
void Menu_Null_Info(void)
{
    LcdWrCmd(CLR_DDRAM_INIT_AC);

    LcdPutStr(0, 1, 0, " In Development");
    LcdPutStr(0, 2, 0, "       :)      ");
//  LcdPutStr(2, 2, 0, "�Է���");
    Delay_Xms(700);

    KeyValue       = KV_NULL;     // ����ȴ�ʱ���µļ�ֵ
    fMenuParamInit = FALSE;       // ��ֹ��ʼ���˵�����, ʹ���ͣ����ԭ��
    fCursorInit    = FALSE;
}

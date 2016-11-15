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

bit fKeyTrig = TRUE;                // 按键触发标志
bit fClrDDRAM = TRUE;               // 屏幕清屏(DDRAM)标志 - 顺便刷新menuName
bit fMenuParamInit = TRUE;          // 菜单参数初始化标志
bit fCursorInit = TRUE;             // 光标位置初始化标志

xdata void(*pfCurrentFunc)(void) = &Menu_Games;     // 指向当前运行的菜单函数的函数指针
//xdata void(*pfCurrentFunc)(void) = &PongGame_Main;     // 指向当前运行的菜单函数的函数指针
//xdata void(*pfCurrentFunc)(void) = &Menu_SplashScreen;     // 指向当前运行的菜单函数的函数指针

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
        param->Cursor = FALSE;                     // 光标清零
    }
    param->PageNo   = FALSE;                     // 页清零
    param->Index    = FALSE;                     // 索引清零
    param->DispNum  = num;                       // 页最多显示项目数
    param->MaxPage  = page;                      // 最多页数
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
    case KV_UP:                   // 向上
        KeyValue = KV_NULL;
        fKeyTrig = TRUE;

        if (param->Cursor != 0)                       // 光标不在顶端
        {
            param->Cursor--;                        // 光标上移
        }
        else                        // 光标在顶端
        {
            if (param->PageNo != 0)                      // 页面没有到最小
            {
                param->PageNo--;                       // 向上翻
            }
            else
            {
                param->Cursor = param->DispNum-1;                      // 光标到底
                param->PageNo = param->MaxPage-1;                  // 最后页
            }
        }
            break;

    case KV_DOWN:                   // 向下
        KeyValue = KV_NULL;
        fKeyTrig = TRUE;

        if (param->Cursor < param->DispNum-1)                          // 光标没有到底,移动光标
        {
            param->Cursor++;                        // 光标向下移动
        }
        else                                                         // 光标到底
        {
            if (param->PageNo < param->MaxPage-1)                      // 页面没有到底,页面移动
            {
                param->PageNo++;                       // 下翻一页
            }
            else                                                     // 页面和光标都到底,返回开始页
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

    case KV_ENTER:                  // 确认
        KeyValue       = KV_NULL;
        fKeyTrig       = TRUE;
        fMenuParamInit = TRUE;
        fClrDDRAM      = TRUE;
        fCursorInit    = TRUE;

        param->Index = param->Cursor + param->PageNo;                   // 计算执行项的索引
//        menuItem[param->Index].CH_PF();                               // 执行相应项
        menuInfo->psItem[param->Index].pfChildFunc();
            break;

    case KV_ESC:                    // 退出
        KeyValue       = KV_NULL;
        fKeyTrig       = TRUE;
        fMenuParamInit = TRUE;
        fClrDDRAM      = TRUE;

        menuInfo->pfFatherFunc();
            break;

    /*case KV_LF:                   // 跳到顶部
        param->Cursor = 0;
        param->PageNo = 0;
            break;

    case KV_RT:                   // 跳到底部
        param->Cursor = param->DispNum-1;                              // 光标到底
        param->PageNo = param->MaxPage-1;                          // 最后页
            break;*/

    default:
//        KeyValue = KV_NULL;
            break;
    }
                          // 返回执行索引
}

/*****************************************************************************
 * Function      : Menu_Process
 * Description   : 处理菜单项
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
        
        Menu_ParamInit(param, PAGE_DISP_NUM, menuInfo->nItems - PAGE_DISP_NUM + 1);                          // 显示项数和页数设置
    }

    if (fKeyTrig)
    {
        fKeyTrig = FALSE;

        Menu_Display(menuInfo->psItem, param->PageNo, param->DispNum, param->Cursor);               // 显示菜单项
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
        KeyValue = KV_NULL;     // 清除等待时按下的键值
        
        SetMenuFlag(TRUE);
        fMenuParamInit = FALSE;       // 防止初始化菜单参数, 使光标停留在原地
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
//  LcdPutStr(2, 2, 0, "吃饭吧");
    Delay_Xms(700);

    KeyValue       = KV_NULL;     // 清除等待时按下的键值
    fMenuParamInit = FALSE;       // 防止初始化菜单参数, 使光标停留在原地
    fCursorInit    = FALSE;
}

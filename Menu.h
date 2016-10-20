    #ifndef __MENU_H__
#define __MENU_H__

/*
    typedef struct _MENU_INFO
    {
        u8 *Title;
        void(*FA_PF)();
        u8 MenuNum;
    } MENU_INFO;
*/

    typedef struct _MENU_ITEM
    {
        u8 *pszMenuName;                    // 菜单项目名称
        void (*pfChildFunc)(void);          // 儿子函数指针 - 要运行的菜单函数
    } MENU_ITEM;

    typedef struct
    {
        u8 *pszTitle;                       // 菜单标题 - 显示在顶端的菜单名称
        void(*pfFatherFunc)(void);
        MENU_ITEM *psItem;
        u8 nItems;
    } MENU_INFO;

    typedef struct _MENU_PRMT
    {
        u8 Cursor;              // 光标值(当前光标位置)
        u8 PageNo;              // 菜单页(显示开始项)
        u8 Index;               // 菜单索引(当前选择的菜单项)

        u8 DispNum;             // 显示项数(每页可以显示菜单项)
        u8 MaxPage;             // 最大页数(最大有多少种显示页)
    } MENU_PRMT;

    #define PAGE_DISP_NUM 3

    extern bit fKeyTrig;                // 按键触发标志
    extern bit fClrDDRAM;               // 屏幕清屏(DDRAM)标志 - 顺便刷新menuName
    extern bit fMenuParamInit;          // 菜单参数初始化标志
    extern bit fCursorInit;             // 光标位置初始化标志
    
    extern xdata void(*pfCurrentFunc)();

    void Func_Null(void);
    void Menu_Null_Info(void);
    void Menu_SystemInfo(void);
    void Menu_Service(void (*func_index)());
    void SetMenuFlag(bit status);

    void Menu_SplashScreen(void);
    void Menu_Main(void);
    void Menu_Games(void);
    void Menu_Setting(void);
    
#endif

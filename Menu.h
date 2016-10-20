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
        u8 *pszMenuName;                    // �˵���Ŀ����
        void (*pfChildFunc)(void);          // ���Ӻ���ָ�� - Ҫ���еĲ˵�����
    } MENU_ITEM;

    typedef struct
    {
        u8 *pszTitle;                       // �˵����� - ��ʾ�ڶ��˵Ĳ˵�����
        void(*pfFatherFunc)(void);
        MENU_ITEM *psItem;
        u8 nItems;
    } MENU_INFO;

    typedef struct _MENU_PRMT
    {
        u8 Cursor;              // ���ֵ(��ǰ���λ��)
        u8 PageNo;              // �˵�ҳ(��ʾ��ʼ��)
        u8 Index;               // �˵�����(��ǰѡ��Ĳ˵���)

        u8 DispNum;             // ��ʾ����(ÿҳ������ʾ�˵���)
        u8 MaxPage;             // ���ҳ��(����ж�������ʾҳ)
    } MENU_PRMT;

    #define PAGE_DISP_NUM 3

    extern bit fKeyTrig;                // ����������־
    extern bit fClrDDRAM;               // ��Ļ����(DDRAM)��־ - ˳��ˢ��menuName
    extern bit fMenuParamInit;          // �˵�������ʼ����־
    extern bit fCursorInit;             // ���λ�ó�ʼ����־
    
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

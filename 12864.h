#ifndef __12864_H__
#define __12864_H__

#include <STC15.h>
#include "Config.h"

sbit LCD_E  = P4^1;
sbit LCD_RS = P3^6;
sbit LCD_RW = P4^2;
sbit PSB    = P1^7;

#define LCD_DATA_PORT P2

/*****LCD功能指令*****/

#define CLR_DDRAM_INIT_AC   0x01   //清屏指令：清屏且AC值为00H
#define INIT_AC             0x02   //将AC设置为00H。且游标移到原点位置

/*----------------------------------------------------
指定在資料的讀取與寫入時，設定游標的移動方向及指定顯示的移位
    0000 01 I/D S
    
I/D :位址計數器遞增遞減選擇
    當 I/D = "1", 游標右移,DDRAM 位址計數器（AC）加1
    當 I/D = "0", 游標左移,DDRAM 位址計數器（AC）減1
S: 顯示畫面整體位移
    S    I/D     DESCRIPTION
    H     H      畫面整體左移
    H     L      畫面整體右移
    
< 写入或读取数据后才会发生变化 >
------------------------------------------------------*/
#define CURSOR_DIR_L    0x04    //游标左移，图像整体不动
#define CURSOR_DIR_R    0x06    //游标右移，图像整体不动
#define SHIFT_DIR_L     0x07    //游标右移，图像整体左移
#define SHIFT_DIR_R     0x05    //游标左移，图像整体右移

/*-----------------------
    顯示狀態 開/關
    0000 1DCB
    
    D=1: 整體顯示ON
    C=1: 游標ON
    B=1: 游標位置反白ON
-------------------------*/
#define DISP_OFF_CURSOR_OFF_BLINK_OFF       0x08    //显示关
#define DISP_ON_CURSOR_OFF_BLINK_OFF        0x0C    //显示开
#define DISP_ON_CURSOR_ON_BLINK_OFF         0x0E    //显示开，游标开
#define DISP_ON_CURSOR_ON_BLINK_ON          0x0F    //显示开，游标开，游标位置反白
#define DISP_ON_CURSOR_OFF_BLINK_ON         0x0D    //显示开，游标关，游标位置反白

/*----------------------------------------------------------------
    游標或顯示移位控制
    0001  S/C  R/L  x  x

    S/C  R/L     Description                      AC Value
     L    L      游標向左移動                      AC = AC - 1
     L    H      游標向右移動                      AC = AC + 1
     H    L      顯示向左移動,且游標跟這移動        AC = AC
     H    H      顯示向右移動,且游標跟這移動        AC = AC
------------------------------------------------------------------*/
#define CURSOR_MOVE_L   0x10
#define CURSOR_MOVE_R   0x14
#define SHIFT_L         0x18
#define SHIFT_R         0x1c

/*---------------------------------------------------------------------------------
        功能設定
      001 DL x RE x x
      
DL : 4/8 BIT 界面控制位元
    當 DL = "1", 為8 BIT MPU 控制界面
    當 DL = "0", 為4 BIT MPU 控制界面
RE : 指令集選擇控制位元
    當 RE = "1", 為擴充指令集動作
    當 RE = "0", 為基本指令集動作
    
< 同一指令之動作不可同時改變 RE 及 DL 需先改變 DL 後在改變 RE 才可確保FLAG 正確設定 >
-----------------------------------------------------------------------------------*/
#define MODE_4BIT_BAS       0x20    //4-bit MPU bus interface, Basic Instrction
#define MODE_4BIT_EXT       0x24    //4-bit MPU bus interface, Basic Instrction
#define MODE_8BIT_BAS       0x30    //8-bit MPU bus interface, Extended Instrction
#define MODE_8BIT_EXT       0x34    //8-bit MPU bus interface, Extended Instrction
#define EN_BAS_INSTRUCTION   0x30    //默认8-bit
#define EN_EXT_INSTRUCTION   0x34    //默认8-bit

/*------------------------------------------------
    設定CGRAM 位址到位址計數器(AC)
    
    01 AC5 AC4 AC3 AC2 AC1 AC0
    
< AC 範圍為 00H..3FH >
< 需確認擴充指令中SR=0 (捲動位址或RAM 位址選擇) >
--------------------------------------------------*/
#define ADDR_SET_CGRAM      0x40   //设置AC，范围为：00H~3FH

/*-------------------------------------------------------
    設定DDRAM 位址到位址計數器(AC)
    
    1 AC6 AC5 AC4 AC3 AC2 AC1 AC0
    < AC6 固定為0 >
    
    DDRAM地址

    0x80  0x81  0x82  0x83  0x84  0x85  0x86  0x87      //第一行汉字位置
    0x90  0x91  0x92  0x93  0x94  0x95  0x96  0x97      //第二行汉字位置
    0x88  0x89  0x8a  0x8b  0x8c  0x8d  0x8e  0x8f      //第三行汉字位置
    0x98  0x99  0x9a  0x9b  0x9c  0x9d  0x9e  0x9f      //第四行汉字位置
---------------------------------------------------------*/
#define ADDR_SET_DDRAM      0x80


    /*--------------------*/
    /*以下为扩充指令集 RE=1*/
    /*--------------------*/

/*------------------------------------------------
    捲動位址或RAM位址選擇
    
    0 0 0 0 0 0 1 SR
    
    SR=1: 允許輸入垂直捲動位址
    SR=0: 允許輸入IRAM位址(擴充指令)
    SR=0: 允許設定CGRAM位址(基本指令)
--------------------------------------------------*/
#define EXT_EN_SET_SCROLL   0x03
#define EXT_EN_SET_IRAM     0x02
#define EXT_EN_SET_CGRAM    0x02

/*------------------------------------------------------------
    反白選擇
    
    0 0 0 0 0 1 R1 R0
    
    選擇4行中的任意一行作反白顯示，並可決定反白與否
    R1,R0 初值為 00 當第一次設定時為反白顯示再一次設定時為正常顯示
--------------------------------------------------------------*/
#define EXT_REVERSE_1       0x04
#define EXT_REVERSE_2       0x05
#define EXT_REVERSE_3       0x06
#define EXT_REVERSE_4       0x07

/*----------------------------
    擴充功能設定
    
    0 0 1 DL X RE G 0
    
    DL=1: 8-BIT 控制介面
    DL=0: 4-BIT 控制介面
    RE=1: 擴充指令集動作
    RE=0: 基本指令集動作
    G=1 : 繪圖顯示ON
    G=0 : 繪圖顯示OFF
------------------------------*/
#define EXT_GRAPH_ON    0x36
#define EXT_GRAPH_OFF   0x34

/*----------------------------------
    設定IRAM 位址或捲動位址
    
    0 1 AC5 AC4 AC3 AC2 AC1 AC0
    
    SR=1: AC5-AC0 為垂直捲動位址
    SR=0: AC3-AC0 為ICON RAM 位址
------------------------------------*/
#define EXT_ADDR_SET_IRAM   0x40
#define EXT_ADDR_SET_SCROLL 0x40

/*----------------------------------
    設定繪圖RAM位址
    
    1 AC6 AC5 AC4 AC3 AC2 AC1 AC0
    
    設定GDRAM位址到位址計數器（AC）
    先設垂直位址再設水平位址(連續寫入兩個位元組的資料來完成垂直與水平的座標位址)
    垂直位址範圍 AC6...AC0
    水平位址範圍 AC3...AC0
------------------------------------*/
#define EXT_ADDR_SET_GDRAM  0x80


/*---------------------------------------------------------------------
    RS    RW    Description
    L     L     MPU write instruction to instruction register (IR)
    L     H     MPU read busy flag (BF) and address counter (AC)
    H     L     MPU write data to data register (DR)
    H     H     MPU read data from data register (DR)
----------------------------------------------------------------------*/
#define CMD     0
#define DAT     1
#define WR      0
#define RD      1
#define LcdWrCmd(byte)  LcdOperateByte(CMD,WR,byte)
#define LcdWrDat(byte)  LcdOperateByte(DAT,WR,byte)
#define LcdRdCmd(void)  LcdOperateByte(CMD,RD,0)        //读出忙信号(BF)和地址计数器(AC)的值
#define LcdRdDat(void)  LcdOperateByte(DAT,RD,0)        //MPU read data from data register (DR),<需要读两遍才能读出正确数据>

void LcdRdBF();
void Lcd_Init();
u8 LcdOperateByte(bit rs, bit rw, u8 byte);
void LcdClrGDRAM(void);


typedef enum {
    OFF,
    ON,
} PIXEL_STATE;
void LcdDrawPoint( u8 X, u8 Y, PIXEL_STATE color );
void LcdDrawFigure(u8 X, u8 Y, u8 DispFigure, PIXEL_STATE color);
//void LcdDrawStrNum(u8 X, u8 Y, bit color, u32 DispFigure);
void LcdDrawStrNum(u8 x, u8 y, u8 length, u32 dispFigure, PIXEL_STATE color);

void LcdPutStr(u8 col, u8 row, bit reverse, u8 code *str);

void LcdDrawLineX(u8 X0, u8 X1, u8 Y, PIXEL_STATE Color);
void LcdDrawLineY(u8 X, u8 Y0, u8 Y1, PIXEL_STATE Color);
void LcdDrawLine(u8 StartX, u8 StartY, u8 EndX, u8 EndY, PIXEL_STATE Color);

void LcdDrawCircle(u8 xc, u8 yc, u8 r, bit fill, PIXEL_STATE color);
// void LcdDrawOval(int x0, int y0, int rx, int ry,int mode);   //RESERVED
void LcdDrawRec(u8 x1, u8 y1, u8 x2, u8 y2, PIXEL_STATE color);

void LcdDrawG_12864(u8 code *ptr);

#endif

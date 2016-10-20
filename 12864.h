#ifndef __12864_H__
#define __12864_H__

#include <STC15.h>
#include "Config.h"

sbit LCD_E  = P4^1;
sbit LCD_RS = P3^6;
sbit LCD_RW = P4^2;
sbit PSB    = P1^7;

#define LCD_DATA_PORT P2

/*****LCD����ָ��*****/

#define CLR_DDRAM_INIT_AC   0x01   //����ָ�������ACֵΪ00H
#define INIT_AC             0x02   //��AC����Ϊ00H�����α��Ƶ�ԭ��λ��

/*----------------------------------------------------
ָ�����Y�ϵ��xȡ�c����r���O���Θ˵��Ƅӷ���ָ���@ʾ����λ
    0000 01 I/D S
    
I/D :λַӋ�����f���f�p�x��
    �� I/D = "1", �Θ�����,DDRAM λַӋ������AC����1
    �� I/D = "0", �Θ�����,DDRAM λַӋ������AC���p1
S: �@ʾ�������wλ��
    S    I/D     DESCRIPTION
    H     H      �������w����
    H     L      �������w����
    
< д����ȡ���ݺ�Żᷢ���仯 >
------------------------------------------------------*/
#define CURSOR_DIR_L    0x04    //�α����ƣ�ͼ�����岻��
#define CURSOR_DIR_R    0x06    //�α����ƣ�ͼ�����岻��
#define SHIFT_DIR_L     0x07    //�α����ƣ�ͼ����������
#define SHIFT_DIR_R     0x05    //�α����ƣ�ͼ����������

/*-----------------------
    �@ʾ��B �_/�P
    0000 1DCB
    
    D=1: ���w�@ʾON
    C=1: �Θ�ON
    B=1: �Θ�λ�÷���ON
-------------------------*/
#define DISP_OFF_CURSOR_OFF_BLINK_OFF       0x08    //��ʾ��
#define DISP_ON_CURSOR_OFF_BLINK_OFF        0x0C    //��ʾ��
#define DISP_ON_CURSOR_ON_BLINK_OFF         0x0E    //��ʾ�����α꿪
#define DISP_ON_CURSOR_ON_BLINK_ON          0x0F    //��ʾ�����α꿪���α�λ�÷���
#define DISP_ON_CURSOR_OFF_BLINK_ON         0x0D    //��ʾ�����α�أ��α�λ�÷���

/*----------------------------------------------------------------
    �Θ˻��@ʾ��λ����
    0001  S/C  R/L  x  x

    S/C  R/L     Description                      AC Value
     L    L      �Θ������Ƅ�                      AC = AC - 1
     L    H      �Θ������Ƅ�                      AC = AC + 1
     H    L      �@ʾ�����Ƅ�,���Θ˸��@�Ƅ�        AC = AC
     H    H      �@ʾ�����Ƅ�,���Θ˸��@�Ƅ�        AC = AC
------------------------------------------------------------------*/
#define CURSOR_MOVE_L   0x10
#define CURSOR_MOVE_R   0x14
#define SHIFT_L         0x18
#define SHIFT_R         0x1c

/*---------------------------------------------------------------------------------
        �����O��
      001 DL x RE x x
      
DL : 4/8 BIT �������λԪ
    �� DL = "1", ��8 BIT MPU ���ƽ���
    �� DL = "0", ��4 BIT MPU ���ƽ���
RE : ָ��x�����λԪ
    �� RE = "1", ��U��ָ�����
    �� RE = "0", �����ָ�����
    
< ͬһָ��֮��������ͬ�r��׃ RE �� DL ���ȸ�׃ DL ���ڸ�׃ RE �ſɴ_��FLAG ���_�O�� >
-----------------------------------------------------------------------------------*/
#define MODE_4BIT_BAS       0x20    //4-bit MPU bus interface, Basic Instrction
#define MODE_4BIT_EXT       0x24    //4-bit MPU bus interface, Basic Instrction
#define MODE_8BIT_BAS       0x30    //8-bit MPU bus interface, Extended Instrction
#define MODE_8BIT_EXT       0x34    //8-bit MPU bus interface, Extended Instrction
#define EN_BAS_INSTRUCTION   0x30    //Ĭ��8-bit
#define EN_EXT_INSTRUCTION   0x34    //Ĭ��8-bit

/*------------------------------------------------
    �O��CGRAM λַ��λַӋ����(AC)
    
    01 AC5 AC4 AC3 AC2 AC1 AC0
    
< AC ������ 00H..3FH >
< ��_�J�U��ָ����SR=0 (�Ԅ�λַ��RAM λַ�x��) >
--------------------------------------------------*/
#define ADDR_SET_CGRAM      0x40   //����AC����ΧΪ��00H~3FH

/*-------------------------------------------------------
    �O��DDRAM λַ��λַӋ����(AC)
    
    1 AC6 AC5 AC4 AC3 AC2 AC1 AC0
    < AC6 �̶���0 >
    
    DDRAM��ַ

    0x80  0x81  0x82  0x83  0x84  0x85  0x86  0x87      //��һ�к���λ��
    0x90  0x91  0x92  0x93  0x94  0x95  0x96  0x97      //�ڶ��к���λ��
    0x88  0x89  0x8a  0x8b  0x8c  0x8d  0x8e  0x8f      //�����к���λ��
    0x98  0x99  0x9a  0x9b  0x9c  0x9d  0x9e  0x9f      //�����к���λ��
---------------------------------------------------------*/
#define ADDR_SET_DDRAM      0x80


    /*--------------------*/
    /*����Ϊ����ָ� RE=1*/
    /*--------------------*/

/*------------------------------------------------
    �Ԅ�λַ��RAMλַ�x��
    
    0 0 0 0 0 0 1 SR
    
    SR=1: ���Sݔ�봹ֱ�Ԅ�λַ
    SR=0: ���Sݔ��IRAMλַ(�U��ָ��)
    SR=0: ���S�O��CGRAMλַ(����ָ��)
--------------------------------------------------*/
#define EXT_EN_SET_SCROLL   0x03
#define EXT_EN_SET_IRAM     0x02
#define EXT_EN_SET_CGRAM    0x02

/*------------------------------------------------------------
    �����x��
    
    0 0 0 0 0 1 R1 R0
    
    �x��4���е�����һ���������@ʾ���K�ɛQ�������c��
    R1,R0 ��ֵ�� 00 ����һ���O���r�鷴���@ʾ��һ���O���r�������@ʾ
--------------------------------------------------------------*/
#define EXT_REVERSE_1       0x04
#define EXT_REVERSE_2       0x05
#define EXT_REVERSE_3       0x06
#define EXT_REVERSE_4       0x07

/*----------------------------
    �U�书���O��
    
    0 0 1 DL X RE G 0
    
    DL=1: 8-BIT ���ƽ���
    DL=0: 4-BIT ���ƽ���
    RE=1: �U��ָ�����
    RE=0: ����ָ�����
    G=1 : �L�D�@ʾON
    G=0 : �L�D�@ʾOFF
------------------------------*/
#define EXT_GRAPH_ON    0x36
#define EXT_GRAPH_OFF   0x34

/*----------------------------------
    �O��IRAM λַ��Ԅ�λַ
    
    0 1 AC5 AC4 AC3 AC2 AC1 AC0
    
    SR=1: AC5-AC0 �鴹ֱ�Ԅ�λַ
    SR=0: AC3-AC0 ��ICON RAM λַ
------------------------------------*/
#define EXT_ADDR_SET_IRAM   0x40
#define EXT_ADDR_SET_SCROLL 0x40

/*----------------------------------
    �O���L�DRAMλַ
    
    1 AC6 AC5 AC4 AC3 AC2 AC1 AC0
    
    �O��GDRAMλַ��λַӋ������AC��
    ���O��ֱλַ���Oˮƽλַ(�B�m����ɂ�λԪ�M���Y�ρ���ɴ�ֱ�cˮƽ������λַ)
    ��ֱλַ���� AC6...AC0
    ˮƽλַ���� AC3...AC0
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
#define LcdRdCmd(void)  LcdOperateByte(CMD,RD,0)        //����æ�ź�(BF)�͵�ַ������(AC)��ֵ
#define LcdRdDat(void)  LcdOperateByte(DAT,RD,0)        //MPU read data from data register (DR),<��Ҫ��������ܶ�����ȷ����>

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

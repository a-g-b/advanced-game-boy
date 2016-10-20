#include <MATH.H>
#include <INTRINS.H>

#include "12864.h"
#include "Delay.h"
#include "Uart.h"

//������ʾ����0-9,ÿ������7��*5�У���һ���Ŵ���Ϊһ���ֽڣ����λΪ0
u8 code Figure75[10][5]=
{
    {0x3E,0x51,0x49,0x45,0x3E},//0
    {0x00,0x42,0x7F,0x40,0x00},//1
    {0x42,0x61,0x51,0x49,0x46},//2
    {0x21,0x41,0x45,0x2B,0x31},//3                                                                                                                              
    {0x18,0x14,0x12,0x7F,0x10},//4
    {0x27,0x45,0x45,0x45,0x39},//5
    {0x3C,0x4A,0x49,0x49,0x30},//6
    {0x01,0x71,0x09,0x05,0x03},//7
    {0x36,0x49,0x49,0x49,0x36},//8
    {0x06,0x49,0x49,0x29,0x1E},//9
};

u8 LcdOperateByte(bit rs, bit rw, u8 byte) {
    LcdRdBF(); 
    LCD_RS = rs;
    LCD_RW = rw;
    
    if (rw == RD) {
        LCD_DATA_PORT = 0xFF;   //׼˫���(������)���ⲿ״̬ǰ,��Ҫ������Ϊ"1",�ſɶ����ⲿ����ȷ״̬
        LCD_E = 1;
        
        Delay_Xus(1);           //Tpw : Enable Pulse Width
        
        byte = LCD_DATA_PORT;   //rs = 0 : ������ַ��������AC����ֵ, rs = 1 : �����ڲ�RAM��data
        
        LCD_E = 0;
        Delay_Xus(1);           //Tc : Enable Cycle Time
        
        return byte;            //���ض�����ֵ
    }
    else {  //rw = WR
        LCD_E = 1;
        
        Delay_Xus(1);           //Tpw : Enable Pulse Width
        
        LCD_DATA_PORT = byte;

        // UartSendDebugData(byte, HEX);
    }
    
    LCD_E = 0;
    Delay_Xus(1);               //Tc : Enable Cycle Time
    
    return 0;
}

/*******************************************************
* �� �� ��:void LcdRdBF()
* ��������:Lcd read busy flag
* �������:��
* �� �� ֵ:��
*******************************************************/
void LcdRdBF()
{
    u8 BF = 0;
    
    LCD_RS = CMD;
    LCD_RW = RD;
    LCD_DATA_PORT = 0xFF;
    do                  //��æ
    {
        LCD_E = 1;
        Delay_Xus(1);   //Tpw : Enable Pulse Width
        BF = LCD_DATA_PORT;  //��LCD�ж��������ݸ�ֵ��BF
        LCD_E = 0;
    }
    while (BF & 0x80);  //BF�����λ:Ϊ1��æ,Ϊ0�����
    
    Delay_Xus(1);       //Tc : Enable Cycle Time
}

void Lcd_Init() 
{
    LcdWrCmd(MODE_8BIT_BAS);
    LcdWrCmd(EN_BAS_INSTRUCTION);
    LcdWrCmd(DISP_ON_CURSOR_OFF_BLINK_OFF);
    LcdWrCmd(CLR_DDRAM_INIT_AC);
    LcdWrCmd(CURSOR_DIR_R);
    LcdWrCmd(EN_EXT_INSTRUCTION);
    LcdClrGDRAM();
    LcdWrCmd(EN_BAS_INSTRUCTION);

    UartSendDebugInfo("LCD     Init_Successfully :)\r\n");
}

/*******************************************************
* �� �� ��: void LcdSetPos(u8 col, u8 row)
* ��������: �趨��ʾλ�� 
* �������: x (0~3)��
            y (0~7)��
* �� �� ֵ: ��
*******************************************************/
static void LcdSetPos(u8 col, u8 row)  
{  
    u8 pos;
    
    if      (row == 0)  row = 0x80;
    else if (row == 1)  row = 0x90;
    else if (row == 2)  row = 0x88;  
    else if (row == 3)  row = 0x98;
    
    pos = row + col;  
    LcdWrCmd(pos);//��д������ǰ��ָ����ʾ��ַ   
}

static void LcdPutStrDat(u8 code *str)
{
    while (*str != '\0')
    {
        LcdWrDat(*str ++);
    }
}

void LcdPutStr(u8 col, u8 row, bit reverse, u8 code *str)
{
    u8 i, j;
    
    if (reverse)
    {
        LcdWrCmd(EN_BAS_INSTRUCTION);
        
        LcdSetPos(col, row);
        LcdPutStrDat(str);
        
        LcdWrCmd(EN_EXT_INSTRUCTION);
        
        /*for (i = 0; i < 16; i ++)
        {
            LcdDrawLineX(0, 127, ((row << 4) + i), 2);
        }*/
        
        switch (row)
        {
        case 0:
            col = 0;
            row = 0;
                break;
        case 1:
            col = 0;
            row = 16;
                break;
        case 2:
            col = 8;
            row = 0;
                break;
        case 3:
            col = 8;
            row = 16;
                break;
        }
        
        for (j = 0; j < 16; j ++)
        {
            LcdWrCmd(EXT_ADDR_SET_GDRAM + row + j);
            LcdWrCmd(EXT_ADDR_SET_GDRAM + col);
            for (i = 0; i < 16; i ++)
            {
                LcdWrDat(0xFF);
            }
        }
    }
    else
    {
        LcdSetPos(col, row);
        LcdPutStrDat(str);
    }
}


/******************************************************
* �� �� ��:void LcdClrGDRAM(void)
* ��������:�����ͼ��ʾ
* �������:��
* �� �� ֵ:��
*******************************************************/
void LcdClrGDRAM(void)
{
    u8 x, y;

    UartSendDebugInfo("LCD Clear GDRAM\r\n");

    for (y = 0; y < 64; y++) 
    {
        LcdWrCmd(EXT_ADDR_SET_GDRAM + y);
        LcdWrCmd(EXT_ADDR_SET_GDRAM);
        for (x = 0; x < 32; x++)
        {
            LcdWrDat(0x00);
            // LcdWrDat(0xff);
        }
    }
}

/*******************************************************
* �� �� ��:void LcdDrawPoint( u8 X, u8 Y, u8 color )
* ��������:���ƶ�λ�û�һ�����صĵ�
* �������:u8 X, u8 Y, u8 color
*          XΪ�У�YΪ�У�COLORΪ����ʾ����ʾ���߷�ת��0��1��2��
* �� �� ֵ:��
*******************************************************/
void LcdDrawPoint(u8 X, u8 Y, PIXEL_STATE color)
{
    u8 Row, Tier, Tier_bit;
    u8 ReadOldH, ReadOldL;
    Tier = X >> 4 ;
    Tier_bit = X & 0x0f;
    if (Y < 32)
    {
        Row = Y ;
    }
    else
    {
        Row = Y - 32 ;
        Tier += 8 ;
    }
    LcdWrCmd(Row + EXT_ADDR_SET_GDRAM);     //д��������
    LcdWrCmd(Tier + EXT_ADDR_SET_GDRAM);    //д��������
    LcdRdDat();
    ReadOldH = LcdRdDat() ;     //����ԭʼ����
    ReadOldL = LcdRdDat() ; 
    LcdWrCmd(Row + EXT_ADDR_SET_GDRAM);
    LcdWrCmd(Tier + EXT_ADDR_SET_GDRAM);
    if  (Tier_bit < 8)
    {
        switch(color)
        {
            case 0 : ReadOldH &= ( ~( 0x01 << ( 7 - Tier_bit ))) ; break ;  //����ʾ
            case 1 : ReadOldH |= ( 0x01 << ( 7 - Tier_bit )) ; break ;      //��ʾ
            case 2 : ReadOldH ^= ( 0x01 << ( 7 - Tier_bit ))    ; break ;   //��ת
            default : break ;    
        }
        LcdWrDat(ReadOldH);  //д��������
        LcdWrDat(ReadOldL);
    }
    else
    {
        switch(color)
        {
            case 0 : ReadOldL &= (~( 0x01 << ( 15 - Tier_bit ))) ; break ;
            case 1 : ReadOldL |= ( 0x01 << ( 15 - Tier_bit ))    ; break ;
            case 2 : ReadOldL ^= ( 0x01 << ( 15 - Tier_bit )) ; break ;
            default : break ;
        }
        LcdWrDat(ReadOldH);
        LcdWrDat(ReadOldL);
    }
}

/*******************************************************
* �� �� ��:void LcdDrawFigure( u8 X, u8 Y, u8 Figure )
* ��������:���ƶ�λ�û����֣�7*5�����ص�
* �������:u8 X, u8 Y, u8 DispFigure
*          XΪ�У�YΪ�У� color ��ʾ��ɫֻ��Ϊ�ڰ�, FigureΪ����ʾ������
* �� �� ֵ:��
*******************************************************/
void LcdDrawFigure(u8 X, u8 Y, u8 DispFigure, PIXEL_STATE color)
{
    u8 i = 0, j = 0;
    for (j = 0; j < 5; j ++)
    {
        for (i = 0; i < 7; i ++)
        {
            if (color)
            {
                LcdDrawPoint(X + j, Y + i, (Figure75[DispFigure][j] >> i) & 0x01);
            }
            else
            {
                LcdDrawPoint(X + j, Y + i, 0);
            }
        }
    }
}

/*****************************************************************************
 * Function      : _ten_to_the_power_of
 * Description   : 
 * Param         : u8  exponent  
 * Returns       : u32 product
 *
 * Record        :
 * 1.Date        : 2016/09/20
 *   Author      : Mr_Liang
 *   Modification: Created function
*****************************************************************************/
static u32 _ten_to_the_power_of(u8 exponent)
{
    u32 xdata product = 1;
    
    for ( ; exponent > 0; --exponent) {
        product *= 10; 
    }

    return product;
}

/*****************************************************************************
 * Function      : LcdDrawStrNum
 * Description   : ���ƶ�λ�û����ֵ��ַ�����7*5�����ص�
 * Param         : u8 X               
                   u8 Y               
                   PIXEL_STATE color  
                   u8 length          
                   u32 dispFigure     
 * Returns       : 
 *
 * Record        :
 * 1.Date        : 2016/08/24
 *   Author      : Mr_Liang
 *   Modification: Created function
 *
 * 2.Date        : 2016/09/20
 *   Author      : Mr_Liang
 *   Modification: Version_2
*****************************************************************************/
void LcdDrawStrNum(u8 x, u8 y, u8 length, u32 dispFigure, PIXEL_STATE color)
{
    u8 xdata digit[10] = 0;
    u8 xdata i;
    bit      flag = 0;

    if (length > 10) {
        length = 10;
    }

    for (i = length; i > 0; --i) 
    {
        digit[(i - 1)] = dispFigure % _ten_to_the_power_of(i) / _ten_to_the_power_of(i - 1);
        if (flag != 1) 
        {
            if (digit[(i - 1)] == 0 && length != 1) {   // length != 1 because input 0, opt 0; 
                --length;
            }
            else {
                flag = 1;
            }
        }
    }

    for (i = 0; i < length; ++i)   
    {
        LcdDrawFigure(x, y, digit[i], color);
        x -= 6;                 // ÿ�����ֵļ�� >= 5
    }
    
}

#if defined(USE_LCD_DRAW_GRAPH)

/*******************************************************
* �� �� ��:void LcdDrawLineX( u8 X0, u8 X1, u8 Y, u8 color )
* ��������:���ƶ�λ�û�һ��ˮƽ��
* �������:u8 X0, u8 X1, u8 Y, u8 color
*          X0�е���ʼλ�ã�X1�е���ֹλ�ã�Y��λ�ã�COLOR
* �� �� ֵ:��
*******************************************************/
void LcdDrawLineX(u8 X0, u8 X1, u8 Y, PIXEL_STATE color)
{    
    u8 Temp ;
    
    if (X0 > X1)
    {
        Temp = X1 ;
        X1 = X0 ;
        X0 = Temp ;
    }
    for ( ; X0 <= X1 ; X0++ )
    LcdDrawPoint(X0, Y, color) ;
}

/*******************************************************
* �� �� ��:void LcdDrawLineY( u8 X, u8 Y0, u8 Y1, u8 color )
* ��������:���ƶ�λ�û�һ����ֱ��
* �������:u8 X, u8 Y0, u8 Y1, u8 color
*          X��λ�ã�Y0�е���ʼλ�ã�Y1�е���ֹλ�ã�COLOR
* �� �� ֵ:��
*******************************************************/
void LcdDrawLineY(u8 X, u8 Y0, u8 Y1, PIXEL_STATE color)
{
    u8 Temp;
    
    if (Y0 > Y1)
    {
        Temp = Y1 ;
        Y1 = Y0 ;
        Y0 = Temp ;
    }
    for (; Y0 <= Y1 ; Y0++)
        LcdDrawPoint(X, Y0, color);
}

/*******************************************************
* �� �� ��: void DrawLine( u8 StartX, u8 StartY, u8 EndX, u8 EndY, u8 color )
* ��������: ����㣬�յ�֮�仭һ��ֱ��
* �������: u8 StartX, u8 StartY, u8 EndX, u8 EndY, u8 color
* �� �� ֵ: ��
*******************************************************/
void LcdDrawLine(u8 StartX, u8 StartY, u8 EndX, u8 EndY, PIXEL_STATE color)
{
    s8 t, distance;      /*������Ļ��С�ı��������(���Ϊint��)*/
    s8 x = 0 , y = 0 , delta_x, delta_y ;
    s8 incx, incy ;
    
    delta_x = EndX - StartX ;
    delta_y = EndY - StartY ;
    if ( delta_x > 0 )
    {
        incx = 1;
    }
    else if ( delta_x == 0 )
    {
        LcdDrawLineY(StartX, StartY, EndY, color) ;
        return ;
    }
    else
    {
        incx = -1 ;
    }
    if ( delta_y > 0 )
    {
        incy = 1 ;
    }
    else if (delta_y == 0 )
    {
        LcdDrawLineX(StartX, EndX, StartY, color) ;    
        return ;
    }
    else
    {
        incy = -1 ;
    }
    delta_x = abs( delta_x );    
    delta_y = abs( delta_y );
    if ( delta_x > delta_y )
    {
        distance = delta_x ;
    }
    else
    {
        distance = delta_y ; 
    }
    LcdDrawPoint( StartX, StartY, color ) ;    
    /* Draw Line*/
    for ( t = 0 ; t <= distance+1 ; t++ )
    {
        LcdDrawPoint( StartX, StartY, color ) ;
        x += delta_x ;
        y += delta_y ;
        if ( x > distance )
        {
            x -= distance ;
            StartX += incx ;
        }
        if ( y > distance )
        {
            y -= distance ;
            StartY += incy ;
        }
    }
}

/*****************************************************************
* �� �� ��: void LcdDrawCircle(u8 xc, u8 yc, u8 r, u8 fill, u8 color)
* ��������: ��ָ��λ�û�һ��ָ����С��Բ
* �������: (x,y) (���ĵ����꣩   r    (�뾶)
*           fill  (Ϊ�Ƿ����)   color(��ɫ)
* �� �� ֵ: ��
******************************************************************/
/*static void _draw_circle_8(u8 xc, u8 yc, u8 x, u8 y, u8 color)   
{
     // ���� c Ϊ��ɫֵ    
     LcdDrawPoint(xc + x, yc + y, color);    
     LcdDrawPoint(xc - x, yc + y, color);    
     LcdDrawPoint(xc + x, yc - y, color);   
     LcdDrawPoint(xc - x, yc - y, color);    
     LcdDrawPoint(xc + y, yc + x, color);   
     LcdDrawPoint(xc - y, yc + x, color);    
     LcdDrawPoint(xc + y, yc - x, color);    
     LcdDrawPoint(xc - y, yc - x, color);    
}
//Bresenham's circle algorithm    
void LcdDrawCircle(u8 xc, u8 yc, u8 r, bit fill, u8 color)
{    
     // (xc, yc) ΪԲ�ģ�r Ϊ�뾶    
     // fill Ϊ�Ƿ����    
     // c Ϊ��ɫֵ       
     // ���Բ��ͼƬ�ɼ������⣬ֱ���˳�    
   
    s8 x = 0, y = r, yi, d;    
        d = 3 - 2 * r;

     if (xc + r < 0 || xc - r >= 127 || yc + r < 0 || yc - r >= 63)   
     {
         return;
     }     
     if (fill)  
     {    
         // �����䣨��ʵ��Բ��    
         while (x <= y)
         {    
             for (yi = x; yi <= y; yi ++)    
             {  
                 _draw_circle_8(xc, yc, x, yi, color);    
             }  
             if (d < 0)   
             {    
                 d = d + 4 * x + 6;    
             }   
             else   
             {    
                 d = d + 4 * (x - y);    
                 y --;    
             }    
   
             x++;    
         }    
     }   
     else   
     {    
         // �������䣨������Բ��    
         while (x <= y)   
         {    
             _draw_circle_8(xc, yc, x, y, color);    
             if (d < 0)   
             {    
                 d = d + 4 * x + 6;    
             }   
             else   
             {    
                 d = d + 4 * (x - y);    
                 y --;    
             }    
             x ++;    
         }    
     }
}  */

/***********************RESERVED*************************
void LcdDrawOval(int x0, int y0, int rx, int ry,int color) 
{
  int OutConst, Sum, SumY;
  int x,y;
  int xOld;
  u32 _rx = rx;
  u32 _ry = ry;
  OutConst = _rx*_rx*_ry*_ry    //Constant as explaint above 
            +(_rx*_rx*_ry>>1);  //To compensate for rounding 
  xOld = x = rx;
  for (y=0; y<=ry; y++) {
    if (y==ry) {
      x=0;
    } else {
      SumY =((int)(rx*rx))*((int)(y*y));    // Does not change in loop 
      while (Sum = SumY + ((int)(ry*ry))*((int)(x*x)),
             (x>0) && (Sum>OutConst)) x--;
    }
    //    Since we draw lines, we can not draw on the first
    //    iteration
    
    if (y) {
      LcdDrawLine(x0-xOld,y0-y+1,x0-x,y0-y,color);
      LcdDrawLine(x0-xOld,y0+y-1,x0-x,y0+y,color);
      LcdDrawLine(x0+xOld,y0-y+1,x0+x,y0-y,color);
      LcdDrawLine(x0+xOld,y0+y-1,x0+x,y0+y,color);
    }
    xOld = x;
  }
}************************************************************/
#endif /* USE_LCD_DRAW_GRAPH */

/*******************************************************************************************
** ��������:  void LcdDrawRec(u8 x1, u8 y1, u8 x2, u8 y2, u8 color)
** ��������:  ������ 
** �䡡��:    u8 x1,u8 y1,u8 x2,u8 y2 ��x1,y1Ϊ���Ͻ����꣬x2,y2Ϊ���½����꣩ 
**         
**        
** �䡡��:    
**         
** ȫ�ֱ���:
** ����ģ��: 
**
** ������:    ���ӥ
** �ա���:     14.03.12
********************************************************************************************/
void LcdDrawRec(u8 x1, u8 y1, u8 x2, u8 y2, PIXEL_STATE color)//�����Σ�x1,y1Ϊ���Ͻ����꣬x2,y2Ϊ���½�����
{
    u8 xdata i;

    for (i=0;i<=x2-x1;i++)
    {  
        LcdDrawPoint(x1+i,y1,color);  
        LcdDrawPoint(x1+i,y2,color);  
    }
    for (i=0;i<=y2-y1;i++)
    {  
        LcdDrawPoint(x1,y1+i,color);  
        LcdDrawPoint(x2,y1+i,color);  
    }
}


void LcdDrawG_12864(u8 code *ptr) 
{
    u8 xdata x, y;
    
    LcdWrCmd(EN_EXT_INSTRUCTION);
    
    LcdWrCmd(EXT_ADDR_SET_GDRAM);
    LcdWrCmd(EXT_ADDR_SET_GDRAM);
    
    for (y = 0; y < 64; y++)
    {
        for (x = 0; x < 16; x++)
        {
            LcdWrDat(*ptr++);
            // Delay_Xms(8);        //�����ع۲�
        }
        
        if(y < 32) 
        {
            LcdWrCmd(EXT_ADDR_SET_GDRAM + y);
            LcdWrCmd(EXT_ADDR_SET_GDRAM);
        }
        else 
        {
            LcdWrCmd(EXT_ADDR_SET_GDRAM + y - 32);
            LcdWrCmd(EXT_ADDR_SET_GDRAM + 8);
        }
//        LED_4 = !LED_4;
    }
    
    LcdWrCmd(EXT_GRAPH_ON);
}

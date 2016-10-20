#include <MATH.H>
#include <INTRINS.H>

#include "12864.h"
#include "Delay.h"
#include "Uart.h"

//用来显示数字0-9,每个数字7行*5列，将一竖排储存为一个字节，最高位为0
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
        LCD_DATA_PORT = 0xFF;   //准双向口(弱上拉)读外部状态前,需要先锁存为"1",才可读到外部的正确状态
        LCD_E = 1;
        
        Delay_Xus(1);           //Tpw : Enable Pulse Width
        
        byte = LCD_DATA_PORT;   //rs = 0 : 读出地址计数器（AC）的值, rs = 1 : 读出内部RAM的data
        
        LCD_E = 0;
        Delay_Xus(1);           //Tc : Enable Cycle Time
        
        return byte;            //返回读到的值
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
* 函 数 名:void LcdRdBF()
* 功能描述:Lcd read busy flag
* 输入参数:无
* 返 回 值:无
*******************************************************/
void LcdRdBF()
{
    u8 BF = 0;
    
    LCD_RS = CMD;
    LCD_RW = RD;
    LCD_DATA_PORT = 0xFF;
    do                  //判忙
    {
        LCD_E = 1;
        Delay_Xus(1);   //Tpw : Enable Pulse Width
        BF = LCD_DATA_PORT;  //将LCD中读到的数据赋值给BF
        LCD_E = 0;
    }
    while (BF & 0x80);  //BF的最高位:为1则忙,为0则空闲
    
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
* 函 数 名: void LcdSetPos(u8 col, u8 row)
* 功能描述: 设定显示位置 
* 输入参数: x (0~3)行
            y (0~7)列
* 返 回 值: 无
*******************************************************/
static void LcdSetPos(u8 col, u8 row)  
{  
    u8 pos;
    
    if      (row == 0)  row = 0x80;
    else if (row == 1)  row = 0x90;
    else if (row == 2)  row = 0x88;  
    else if (row == 3)  row = 0x98;
    
    pos = row + col;  
    LcdWrCmd(pos);//在写入数据前先指定显示地址   
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
* 函 数 名:void LcdClrGDRAM(void)
* 功能描述:清除绘图显示
* 输入参数:无
* 返 回 值:无
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
* 函 数 名:void LcdDrawPoint( u8 X, u8 Y, u8 color )
* 功能描述:在制定位置画一个像素的点
* 输入参数:u8 X, u8 Y, u8 color
*          X为列，Y为行，COLOR为不显示、显示或者翻转（0、1、2）
* 返 回 值:无
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
    LcdWrCmd(Row + EXT_ADDR_SET_GDRAM);     //写入行坐标
    LcdWrCmd(Tier + EXT_ADDR_SET_GDRAM);    //写入列坐标
    LcdRdDat();
    ReadOldH = LcdRdDat() ;     //读出原始数据
    ReadOldL = LcdRdDat() ; 
    LcdWrCmd(Row + EXT_ADDR_SET_GDRAM);
    LcdWrCmd(Tier + EXT_ADDR_SET_GDRAM);
    if  (Tier_bit < 8)
    {
        switch(color)
        {
            case 0 : ReadOldH &= ( ~( 0x01 << ( 7 - Tier_bit ))) ; break ;  //不显示
            case 1 : ReadOldH |= ( 0x01 << ( 7 - Tier_bit )) ; break ;      //显示
            case 2 : ReadOldH ^= ( 0x01 << ( 7 - Tier_bit ))    ; break ;   //翻转
            default : break ;    
        }
        LcdWrDat(ReadOldH);  //写入新数据
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
* 函 数 名:void LcdDrawFigure( u8 X, u8 Y, u8 Figure )
* 功能描述:在制定位置画数字，7*5个像素点
* 输入参数:u8 X, u8 Y, u8 DispFigure
*          X为列，Y为行， color 显示颜色只能为黑白, Figure为待显示的数字
* 返 回 值:无
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
 * Description   : 在制定位置画数字的字符串，7*5个像素点
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
        x -= 6;                 // 每个数字的间隔 >= 5
    }
    
}

#if defined(USE_LCD_DRAW_GRAPH)

/*******************************************************
* 函 数 名:void LcdDrawLineX( u8 X0, u8 X1, u8 Y, u8 color )
* 功能描述:在制定位置画一条水平线
* 输入参数:u8 X0, u8 X1, u8 Y, u8 color
*          X0列的起始位置，X1列的终止位置，Y行位置，COLOR
* 返 回 值:无
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
* 函 数 名:void LcdDrawLineY( u8 X, u8 Y0, u8 Y1, u8 color )
* 功能描述:在制定位置画一条垂直线
* 输入参数:u8 X, u8 Y0, u8 Y1, u8 color
*          X列位置，Y0行的起始位置，Y1行的终止位置，COLOR
* 返 回 值:无
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
* 函 数 名: void DrawLine( u8 StartX, u8 StartY, u8 EndX, u8 EndY, u8 color )
* 功能描述: 在起点，终点之间画一条直线
* 输入参数: u8 StartX, u8 StartY, u8 EndX, u8 EndY, u8 color
* 返 回 值: 无
*******************************************************/
void LcdDrawLine(u8 StartX, u8 StartY, u8 EndX, u8 EndY, PIXEL_STATE color)
{
    s8 t, distance;      /*根据屏幕大小改变变量类型(如改为int型)*/
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
* 函 数 名: void LcdDrawCircle(u8 xc, u8 yc, u8 r, u8 fill, u8 color)
* 功能描述: 在指定位置画一个指定大小的圆
* 输入参数: (x,y) (中心点坐标）   r    (半径)
*           fill  (为是否填充)   color(颜色)
* 返 回 值: 无
******************************************************************/
/*static void _draw_circle_8(u8 xc, u8 yc, u8 x, u8 y, u8 color)   
{
     // 参数 c 为颜色值    
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
     // (xc, yc) 为圆心，r 为半径    
     // fill 为是否填充    
     // c 为颜色值       
     // 如果圆在图片可见区域外，直接退出    
   
    s8 x = 0, y = r, yi, d;    
        d = 3 - 2 * r;

     if (xc + r < 0 || xc - r >= 127 || yc + r < 0 || yc - r >= 63)   
     {
         return;
     }     
     if (fill)  
     {    
         // 如果填充（画实心圆）    
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
         // 如果不填充（画空心圆）    
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
** 函数名称:  void LcdDrawRec(u8 x1, u8 y1, u8 x2, u8 y2, u8 color)
** 功能描述:  画矩形 
** 输　入:    u8 x1,u8 y1,u8 x2,u8 y2 （x1,y1为左上角坐标，x2,y2为右下角坐标） 
**         
**        
** 输　出:    
**         
** 全局变量:
** 调用模块: 
**
** 作　者:    吴鉴鹰
** 日　期:     14.03.12
********************************************************************************************/
void LcdDrawRec(u8 x1, u8 y1, u8 x2, u8 y2, PIXEL_STATE color)//画矩形，x1,y1为左上角坐标，x2,y2为右下角坐标
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
            // Delay_Xms(8);        //静静地观察
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

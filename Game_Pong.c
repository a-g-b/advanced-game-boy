#include "Config.h"
#include "12864.h"
#include "Delay.h"
#include "Timer.h"
#include "Uart.h"
#include "Menu.h"
#include "Key.h"
#include "Game_Snake.h"

#define BL_RADIUS       1                       // radius of the ball
#define PAD_LEN         11                      // (only accept odd number) length of the paddle
#define PAD_OFFSET_Y    ((PAD_LEN - 1) >> 1)    // offset Y

#define PI 3.14159265
#define DEG_TO_RAD(DEG) (float)((DEG) / 180.0 * PI) // convert degree to radian
#define sin_deg(DEG)    sin(DEG_TO_RAD(DEG))
#define cos_deg(DEG)    cos(DEG_TO_RAD(DEG))
#define tan_deg(DEG)    tan(DEG_TO_RAD(DEG))
#define _rdf(VAL)       (u8)((VAL) + 0.5)    // round float and return u8 value    

#define REFLACT_ANGEL_MAX 75

typedef struct
{
    float VecX;
    float VecY;
    float X;
    float Y;
    u8 OldX;
    u8 OldY;
    s8 Angle;
    u8 Speed;
} BALL_STRU;

typedef struct
{
    u8 X;
    u8 Y;
} PAD_STRU;

typedef struct
{
    u8 Score;
    PAD_STRU Pd;
} PLAYER_STRU;

static BALL_STRU   xdata Ball;
static PLAYER_STRU xdata Py1,
                         Py2;

//#define DASH_LINE_SEGMENT_LEN   2
//#define DASH_LINE_GAP_LEN       2

#define VERTICAL    0    
#define HORIZONTAL  1

// obscure code, sorry :(
// only support draw horizontal or vertical line
static void _draw_dashed_line(u8 x0, u8 y0, u8 x1, u8 y1, u8 segLen, u8 gapLen, PIXEL_STATE color)   
{
    bit mode = 0;
    u8  tmp = 0;
    u8  num0, num1;
    
    if (x0 != x1 && y0 != y1)   // neither horizontal nor vertical
    {
        return;
    }
    else if (x0 == x1)      // vertical
    {
        num0 = y0;
        num1 = y1;
        mode = VERTICAL;
    }
    else if (y0 == y1)      // horizontal
    {
        num0 = x0;
        num1 = x1;
        mode = HORIZONTAL;
    }

    if (num0 < num1)        
    {
        tmp  = num0;
        num0 = num1;
        num1 = tmp;
    }

    tmp = 0;
    for (; num0 >= num1; --num0)  // drawing from down to up
    {
        if (num0 == 255)
            break;
        
        ++tmp;
        if (tmp < segLen + 1)
        {
            if (mode == VERTICAL)
            {
                LcdDrawPoint(x0, num0, color);
            }
            else if (mode == HORIZONTAL)
            {
                LcdDrawPoint(num0, y0, color);
            }
        }
        else if (tmp < segLen + gapLen)
        {
            ;
        }
        else 
        {
            tmp = 0;
        }
    }
}

static void DrawAuxiLine(u8 x, u8 y, PIXEL_STATE color)
{
    _draw_dashed_line(x, y, x, 0, 2, 2, color);
    _draw_dashed_line(x, y, 0, y, 2, 2, color);
}

static void DrawMiddleLine(void)
{
    _draw_dashed_line(63, 63, 63, 0, 3, 2, ON);
    _draw_dashed_line(64, 63, 64, 0, 3, 2, ON);
}

#undef VERTICAL      
#undef HORIZONTAL  

static s8 _get_rand_angle(void)
{
    bit sign;
    s8 xdata angle;
    
    srand(TH0 + TL0);
    angle = rand() % REFLACT_ANGEL_MAX;
    srand(TH0 + TL0);
    sign = rand() % 1;

    if (sign == 0)
    {
        angle = 0 - angle;
    }

    return angle;
}

/*
static void _calc_vector(float *pvx, float *pvy, s8 ang)
{
    if (ang >= 0)
    {
        *pvx = cos_deg(ang);
        *pvy = sin_deg(ang);
    }
    else if (ang < 0)
    {
        *pvx = cos_deg(ang);
        *pvy = 0 - sin_deg(ang);
    }
}
*/

#define ANGLE -30

static void PongParamInit(void)
{
    Ball.X = 50.0;
    Ball.Y = 30.0;
//    Ball.VecX = 3.1;
//    Ball.VecY = 3.3;
//    Ball.VecX = 0.55;
//    Ball.VecY = 0.80;
//    Ball.Angle = _get_rand_angle();
    Ball.Angle = -20;
    Ball.VecX = cos_deg(Ball.Angle) * 5;
    Ball.VecY = sin_deg(Ball.Angle) * 5;
//    _calc_vector(&Ball.VecX, &Ball.VecY, Ball.Angle);
    Py1.Pd.X = 122;
    Py1.Pd.Y = 32;
    Py2.Pd.X = 5;
    Py2.Pd.Y = 32;
}

static void _judge_kv_validity()
{
    ;
}

static void PaddleCalcCoor(void)
{
    _judge_kv_validity();
}

static void _judge_collide_wall(u8 rdX, u8 rdY, float *pvx, float *pvy)
{
    if (rdX < 0 + BL_RADIUS || rdX > 127 - BL_RADIUS)
    {
        *pvx = 0 - *pvx;
        UartSendDebugInfo("\r\nBOUNCE");
    }
      
    if (rdY < 0 + BL_RADIUS || rdY > 63 - BL_RADIUS)
    {
        *pvy = 0 - *pvy;
        UartSendDebugInfo("\r\nBOUNCE");
    }
}

static void _judge_collide_pad(float x, float y, float *pvx)
{
    if (x + BL_RADIUS == Py1.Pd.X - 1)   // collide
    {
        if (y > Py1.Pd.Y + PAD_OFFSET_Y && y < Py1.Pd.Y - PAD_OFFSET_Y)
        {
            *pvx = 0 - *pvx;
        }
    }
    else if (x - BL_RADIUS == Py2.Pd.X + 1)
    {
        if (y > Py1.Pd.Y + PAD_OFFSET_Y && y < Py1.Pd.Y - PAD_OFFSET_Y)
        {
            *pvx = 0 - *pvx;
        }
    }
}

static void _judge_collision(float x, float y, float *pvx, float *pvy)
{
    u8 xdata rdX, rdY;
    
    x += *pvx;
    y += *pvy;

    rdX = _rdf(x);
    rdY = _rdf(y);
    
    _judge_collide_wall(rdX, rdY, pvx, pvy);
    _judge_collide_pad(rdX, rdY, pvx);
}

static void BallCalcCoor(void)
{
    _judge_collision(Ball.X, Ball.Y, &Ball.VecX, &Ball.VecY);

    Ball.OldX = _rdf(Ball.X);
    Ball.OldY = _rdf(Ball.Y);

    Ball.X += Ball.VecX;
    Ball.Y += Ball.VecY;
    
    UartSendString("\r\n");
    UartSendFloat(Ball.X);
    UartSendFloat(Ball.Y);
//    UartSendDebugData(Ball.OldX, DEC);
//    UartSendDebugData(Ball.OldY, DEC);
    UartSendDebugData(_rdf(Ball.X), DEC);
    UartSendDebugData(_rdf(Ball.Y), DEC);
}

static void _draw_ball(u8 x, u8 y, PIXEL_STATE color)
{
    u8 i;
    
    LcdDrawPoint(x, y, color);     // center of the circle
    
/*
    for (i = 0; i < (BL_RADIUS << 1) + 1; ++i)
    {
        LcdDrawPoint(x - BL_RADIUS + i, y - BL_RADIUS - 1, color); // up
        LcdDrawPoint(x - BL_RADIUS + i, y + BL_RADIUS + 1, color); // down
        LcdDrawPoint(x - BL_RADIUS - 1, y - BL_RADIUS + i, color); // left
        LcdDrawPoint(x + BL_RADIUS + 1, y - BL_RADIUS + i, color); // right
    }
*/
    for (i = 0; i < 3; ++i)
    {
        LcdDrawPoint(x    , y + i - 1, color);
        LcdDrawPoint(x - 1, y + i - 1, color);
        LcdDrawPoint(x + 1, y + i - 1, color);
    }
}

static void BallDisp(void)
{
    if (Ball.OldX == _rdf(Ball.X) 
        && Ball.OldY == _rdf(Ball.Y))
    {
        UartSendDebugInfo("\r\nRETURN");
        return;
    }
    
    _draw_ball(Ball.OldX, Ball.OldY, OFF);
    _draw_ball(_rdf(Ball.X), _rdf(Ball.Y), ON);
    LcdWrCmd(EXT_GRAPH_ON);
}

static void _draw_paddle(u8 x, u8 y)
{
    u8 i;
    
    LcdDrawPoint(x, y, ON);

    for (i = 0; i < PAD_LEN; ++i)
    {
        LcdDrawPoint(x - 1, y + i - PAD_OFFSET_Y, ON);    // left
        LcdDrawPoint(x + 1, y + i - PAD_OFFSET_Y, ON);    // right
    }
    
    LcdDrawPoint(x, y - PAD_OFFSET_Y, ON);    // up
    LcdDrawPoint(x, y + PAD_OFFSET_Y, ON);    // down
}

static void PaddleDisp(void)
{
    _draw_paddle(Py1.Pd.X, Py1.Pd.Y);
    _draw_paddle(Py2.Pd.X, Py2.Pd.Y);
}

void PongGame_Main(void)
{
    u8 xdata Hertz = 0;
    
    pfCurrentFunc = &PongGame_Main;
    
    if (fKeyTrig) 
    {
        fKeyTrig = FALSE;

        SetMenuFlag(FALSE);

        LcdWrCmd(EN_BAS_INSTRUCTION);
        LcdWrCmd(CLR_DDRAM_INIT_AC);
        LcdWrCmd(EN_EXT_INSTRUCTION);
        LcdClrGDRAM();

        PongParamInit();
        PaddleDisp();
        DrawMiddleLine();
        LcdWrCmd(EXT_GRAPH_ON);
    }
    
    FreqCnt = 0;

    PaddleCalcCoor();
    BallCalcCoor();
    PaddleDisp();
    BallDisp();
//    DrawMiddleLine();
    LcdWrCmd(EXT_GRAPH_ON);  xsaA   sssa
//    Delay_Xms(5);

    Hertz = 1000 / FreqCnt;
/*
    UartSendDebugInfo("\r\n");
    UartSendDebugData(Hertz, DEC);  
    UartSendDebugInfo("Hz");
*/
    
//    UartSendDebugData((u8)(sin_deg(30)), DEC);
}

/*  old draw dashed line
    else if (x0 == x1)      // vertical
    {
        if (y0 < y1)        
        {
            tmp = y0;
            y0  = y1;
            y1  = tmp;
        }

        tmp = 0;
        for (; y0 >= y1; --y0)  // drawing from down to up
        {
            if (y0 == 255)
                break;
            ++tmp;
            
            if (tmp < segLen + 1)
            {
                LcdDrawPoint(x0, y0, ON);
            }
            else if (tmp < segLen + gapLen)
            {
                ;
            }
            else 
            {
                tmp = 0;
            }
        }        
    }
    else if (y0 == y1)      // horizontal
    {
        if (x0 < x1)
        {
            tmp = y0;
            y0  = y1;
            y1  = tmp;
        }
        
        tmp = 0;
        for (; x0 >= x1; --x0)  // drawing from down to up
        {
            if (x0 == 255)
                break;
            ++tmp;
            
            if (tmp < segLen + 1)
            {
                LcdDrawPoint(x0, y0, ON);
            }
            else if (tmp < segLen + gapLen)
            {
                ;
            }
            else 
            {
                tmp = 0;
            }
        }   
    }
*/

/* draw ball 5*5
    for (i = 0; i < 5; ++i)
    {
        LcdDrawPoint(x    , y + i - 2, color);
        LcdDrawPoint(x - 1, y + i - 2, color);
        LcdDrawPoint(x + 1, y + i - 2, color);
        LcdDrawPoint(x - 2, y + i - 2, color);
        LcdDrawPoint(x + 2, y + i - 2, color);
    }
*/

/*  test convertion bewteen float and u8
    for (Ball.X = 0; Ball.X < 10; Ball.X += 0.01)
    {
        UartSendDebugInfo("\r\n");
        UartSendFloat(Ball.X);
        UartSendDebugData(_rdf(Ball.X), DEC);
        UartSendFloat((float)_rdf(Ball.X));
    }
*/

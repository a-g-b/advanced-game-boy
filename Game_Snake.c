#include "Config.h"
#include "12864.h"
#include "Delay.h"
#include "Uart.h"
#include "Menu.h"
#include "Timer.h"
#include "Key.h"
#include "Game_Snake.h"

// x-axis   0 - 30
// y-axis   0 - 12

#define X_AXIS_MAX 30
#define Y_AXIS_MAX 12

#define SNAKE_INIT_NODE 3
#define SNAKE_NODE_MAX  100      // max length of snake
#define FOOD_MAX        10       // max #food on the screen at the same time

static u8 code Node_Side[4][4] = 
{
    {0x0f, 0x09, 0x09, 0x09}, // left
    {0x0f, 0x08, 0x08, 0x0f}, // down
    {0x09, 0x09, 0x09, 0x0f}, // right
    {0x0f, 0x01, 0x01, 0x0f}, // up
};

static u8 code Node_Corner[4][4] =      // clockwise
{
    {0x01, 0x03, 0x05, 0x09}, // left
    {0x0f, 0x04, 0x02, 0x01}, // down
    {0x09, 0x0a, 0x0c, 0x08}, // right
    {0x08, 0x04, 0x02, 0x0f}, // up
};

typedef enum
{
    IDLE,
    OCCUPY,
} FOOD_STATE;

typedef struct
{
    u8          X;
    u8          Y;
    FOOD_STATE  State;
} FOOD_STRU;

typedef enum
{
    UP      = 4,
    RIGHT   = 3,
    DOWN    = 2,
    LEFT    = 1,
} NODE_DIR;

typedef enum
{
    SIDE,
    CORNER,
} NODE_PLACE;

typedef enum
{
    NOT_DEF,
    CW,             // clockwise
    COUNTER_CW,    
} ROT_DIR;

typedef struct
{
    NODE_PLACE Place;
    ROT_DIR    R_Dir;
} NODE_PRTY_STRU;

typedef struct 
{
    u8              X;
    u8              Y;
    NODE_DIR        Nd_Dir;
    NODE_PRTY_STRU  Prty;
} SNAKE_NODE_STRU;

typedef struct
{
    SNAKE_NODE_STRU Node[SNAKE_NODE_MAX];
    NODE_DIR        Sk_Dir;    
    u8              nNode;
    u8              Life;
} SNAKE_STRU;

bit fNewFood  = FALSE;

volatile u16 xdata GmScore = 0;
//volatile u16 xdata FreqCnt = 0;

static bit fClrTail = FALSE;

static u8 xdata SkTailCoorX, 
                SkTailCoorY;

static NODE_DIR xdata SkTailDir;

static FOOD_STRU  xdata Food[FOOD_MAX];
static SNAKE_STRU xdata Snake;

static void DrawBoundary(void)
{
//    LcdWrCmd(EN_EXT_INSTRUCTION);
    LcdDrawRec(0, 8, 127, 127, ON);
    LcdDrawRec(1, 9, 126, 126, ON);
    LcdWrCmd(EXT_GRAPH_ON);
//    LcdWrCmd(EN_BAS_INSTRUCTION);
}

typedef enum
{
    UPPER,
    LOWER,
} CANVAS_SIDE;

static void _clr_canvas_side(CANVAS_SIDE side, u8 y)
{
    u8 offset;
    
    if (side == UPPER)
    {
        offset = 0;
    }
    else if (side == LOWER)
    {
        offset = 8;
    }

    LcdWrCmd(EXT_ADDR_SET_GDRAM + y);
    LcdWrCmd(EXT_ADDR_SET_GDRAM + offset);
    LcdWrDat(0xc0);
    LcdWrDat(0x00);
    LcdWrCmd(EXT_ADDR_SET_GDRAM + y);
    LcdWrCmd(EXT_ADDR_SET_GDRAM + 7 + offset);
    LcdWrDat(0x00);
    LcdWrDat(0x03);
}

/*****************************************************************************
 * Function      : ClrCanvas
 * Description   : Clear canvas
 * Param         : void  
 * Returns       : none
 *
 * Record        :
 * 1.Date        : 10/06/2016
 *   Author      : Mr_Liang
 *   Modification: Created function
*****************************************************************************/
static void ClrCanvas(void)
{
    u8 x, y;   
        
    for (y = 10; y < 32; y++)   // Upper half canvas    
    {
        LcdWrCmd(EXT_ADDR_SET_GDRAM + y);
        LcdWrCmd(EXT_ADDR_SET_GDRAM + 1);
        for (x = 0; x < 12; x++)
        {
            LcdWrDat(0x00);
        }

        _clr_canvas_side(UPPER, y);
    }
    for (y = 0; y < 30; y++)    // lower half canvas
    {
        LcdWrCmd(EXT_ADDR_SET_GDRAM + y);
        LcdWrCmd(EXT_ADDR_SET_GDRAM + 9);
        for (x = 0; x < 12; x++)
        {
            LcdWrDat(0x00);
        }

        _clr_canvas_side(LOWER, y);
    }
    
    LcdWrCmd(EXT_GRAPH_ON);
}

static void _draw_block(u8 x, u8 y, PIXEL_STATE color)
{
    x = (x << 2) + 2 + 1;
    y = (y << 2) + 10 + 1;

    LcdDrawPoint(x    , y    , color);
    LcdDrawPoint(x + 1, y    , color);
    LcdDrawPoint(x    , y + 1, color);
    LcdDrawPoint(x + 1, y + 1, color);
}

#define NOT_COINCIDE 0
#define COINCIDE     1

static u8 _food_coinc_food_check(u8 tmpX, u8 tmpY)
{
    u8 i;
    
    for (i = 0; i < FOOD_MAX; ++i)
    {
        if (tmpX == Food[i].X 
            && tmpY == Food[i].Y)
        {
//            UartSendDebugInfo("\r\nRegenerate Food, coincide with other foods ");
//            UartSendDebugData(tmpX, DEC);
//            UartSendDebugData(tmpY, DEC);
            return COINCIDE;   // conincide
        }
    }

    return NOT_COINCIDE;
}

static u8 _food_coinc_snake_check(u8 tmpX, u8 tmpY)
{
    u8 i;
    
    for (i = 0; i < SNAKE_NODE_MAX; ++i)
    {
        if (tmpX == Snake.Node[i].X 
            && tmpY == Snake.Node[i].Y)
        {
//            UartSendDebugInfo("\r\nRegenerate Food, coincide with SNAKE ");
//            UartSendDebugData(tmpX, DEC);
//            UartSendDebugData(tmpY, DEC);
            return COINCIDE;   // conincide
        }
    }

    return NOT_COINCIDE;
}

/*****************************************************************************
 * Function      : _food_coor_calc
 * Description   : Generate food coordinate randomly
 * Param         : u8 nFd  
 * Returns       : void
 *
 * Record        :
 * 1.Date        : 10/14/2016
 *   Author      : Mr_Liang
 *   Modification: Created function
*****************************************************************************/
static void _food_coor_calc(u8 nFd)
{
    u8 tmpX, tmpY;

regenerate:    
    srand(TH0 + TL0);
    tmpX = rand() % 31;
    srand(TH0 + TL0);
    tmpY = rand() % 13;

    if (_food_coinc_food_check(tmpX, tmpY) == COINCIDE 
        || _food_coinc_snake_check(tmpX, tmpY) == COINCIDE)
    {
        goto regenerate;
    }

    Food[nFd].X = tmpX;
    Food[nFd].Y = tmpY;
    Food[nFd].State = OCCUPY;
}

#undef NOT_COINCIDE 
#undef COINCIDE     

/*****************************************************************************
 * Function      : _draw_food
 * Description   : Draw foods
 * Param         : u8 nFd  
 * Returns       : static
 *
 * Record        :
 * 1.Date        : 10/05/2016
 *   Author      : Mr_Liang
 *   Modification: Created function
 *
 * 2.Date        : 10/14/2016
 *   Author      : Mr_Liang
 *   Modification: Foods cannot generate inside the snake or coincide with existing food
*****************************************************************************/
static void _draw_food(u8 nFd)
{
    _food_coor_calc(nFd);
    
/*
    srand(TH0 + TL0);
    Food[nFd].X = rand() % 31;
    srand(TH0 + TL0);
    Food[nFd].Y = rand() % 13;
    Food[nFd].State = OCCUPY;
*/

    _draw_block(Food[nFd].X, Food[nFd].Y, ON);

    LcdWrCmd(EXT_GRAPH_ON);

/*
    // DEBUG
    UartSendDebugInfo("\r\nnFd = ");
    UartSendDebugData(nFd, DEC);
    UartSendDebugData(Food[nFd].X, DEC);
    UartSendDebugData(Food[nFd].Y, DEC);
*/
}

/*****************************************************************************
 * Function      : FoodDisp
 * Description   : 
 * Param         : void  
 * Returns       : none
 *
 * Record        :
 * 1.Date        : 10/05/2016
 *   Author      : Mr_Liang
 *   Modification: Created function
*****************************************************************************/
static void FoodDisp(void)
{
    u8 nIdleFd;

    for (nIdleFd = 0; nIdleFd < FOOD_MAX; ++nIdleFd)
    {
        if (Food[nIdleFd].State == IDLE) {
            break;
        }
    }

    if (nIdleFd < FOOD_MAX && fNewFood == 1) 
    {
        fNewFood = 0;
        _draw_food(nIdleFd);  
    }

/*
    if (KeyValue == KV_UP) {
        KeyValue = KV_NULL;
        
        nIdleFd = rand() % FOOD_MAX;
        Food[nIdleFd].State = IDLE;

//        LcdWrCmd(EN_EXT_INSTRUCTION);
//        LcdClrGDRAM();
//        LcdWrCmd(EN_BAS_INSTRUCTION);
//        
//        UartSendDebugInfo("Clear ");
//        UartSendDebugData(nIdleFd);
//        UartSendDebugInfo("\r\n");

        // Clear all
//        LcdWrCmd(EN_EXT_INSTRUCTION);

        for (nIdleFd = 0; nIdleFd < FOOD_MAX; nIdleFd++) {
            if (Food[nIdleFd].State != IDLE) 
            {
                _draw_block(Food[nIdleFd].X, 
                            Food[nIdleFd].Y, 
                            OFF);                       // eliminate display
                LcdWrCmd(EXT_GRAPH_ON);                 // avoid blink

                Food[nIdleFd].X = 0xff;        // move to outside of canvas
                Food[nIdleFd].Y = 0xff;
                Food[nIdleFd].State = IDLE;    // clear flag
                fNewFood = 0;                           // avoid generate new food after clear canvas

                // DEBUG
                UartSendDebugInfo("Clear Fd");
                UartSendDebugData(nIdleFd, DEC);
//                UartSendDebugData(Food[nIdleFd].X, DEC);
//                UartSendDebugData(Food[nIdleFd].Y, DEC);
                UartSendDebugInfo("\r\n");

            }
        }

        LcdWrCmd(EXT_GRAPH_ON);
//        LcdWrCmd(EN_BAS_INSTRUCTION);
//        LcdWrCmd(EN_EXT_INSTRUCTION);
//        ClrCanvas();
//        LcdWrCmd(EN_BAS_INSTRUCTION);

    }
    
    if (KeyValue == KV_DOWN) {
        KeyValue = KV_NULL;
        fNewFood = 1;
    }
*/
}

/*
static void _draw_str_three_dig(u8 x, u8 y, u16 dispFigure)
{
    u8 xdata digit[3];
    u8 xdata i,
             length = 3;

    digit[2] = dispFigure / 100;
    digit[1] = dispFigure % 100 / 10;
    digit[0] = dispFigure % 10;

    for (i = 2; i > 0; --i)
    {
        if (digit[i] == 0)
            --length;
    }

    for (i = 0; i < length; ++i)   
    {
        LcdDrawFigure(x, y, digit[i], ON);
        x -= 6;                 // 每个数字的间隔 >= 5
    }
}
*/

/*****************************************************************************
 * Function      : DispScore
 * Description   : print score on the screen
 * Param         : void  
 * Returns       : none
 *
 * Record        :
 * 1.Date        : 10/05/2016
 *   Author      : Mr_Liang
 *   Modification: Created function
 *
 * 2.Date        : 10/15/2016
 *   Author      : Mr_Liang
 *   Modification: Optimizing execution time
*****************************************************************************/
static void DataDisp(void)
{
    u8 xdata dig[3] = 0;
    static u8 xdata oldDig[3] = 0;
    
//    LcdDrawStrNum(30, 0, ON, 3, (u32)GmScore);
//    LcdDrawStrNum(80, 0, ON, 3, (u32)Hertz);
//    LcdDrawStrNumThreeDig(30, 0, GmScore + 100);

/*
    if (GmScore != (u16)Snake.nNode)
    {
        GmScore = (u16)Snake.nNode;
        _draw_str_three_dig(30, 0, GmScore + 100);
    }
*/

    GmScore = Snake.nNode - SNAKE_INIT_NODE;
    if (GmScore == 1)
    {
        oldDig[0] = 0;
        oldDig[1] = 0;
        oldDig[2] = 0;
    }
    
    dig[0] = GmScore % 10;
    if (oldDig[0] != dig[0])
    {
        oldDig[0] = dig[0];
        LcdDrawFigure(30, 0, dig[0], ON);
        
        dig[1] = GmScore % 100 / 10;
        if (oldDig[1] != dig[1])
        {
            oldDig[1] = dig[1];
            LcdDrawFigure(24, 0, dig[1], ON);
            
            dig[2] = GmScore / 100;
            if (oldDig[2] != dig[2])
            {
                oldDig[2] = dig[2];
                LcdDrawFigure(18, 0, dig[2], ON);
            }
        }
    }

//    LcdWrCmd(EXT_GRAPH_ON);
//    LcdWrCmd(EN_BAS_INSTRUCTION);
}

/*****************************************************************************
 * Function      : SnakeParamInit
 * Description   : initialize param
 * Param         : void  
 * Returns       : none
 *
 * Record        :
 * 1.Date        : 10/05/2016
 *   Author      : Mr_Liang
 *   Modification: Created function
*****************************************************************************/
static void SnakeParamInit(void)
{
    u8 i;    

/*
    Snake.nNode = SNAKE_INIT_NODE;
    Snake.Sk_Dir = LEFT;

    for (i = 0; i < SNAKE_INIT_NODE; ++i)
    {
        Snake.Node[i].X          = 20 + i;
        Snake.Node[i].Y          = 8;
        Snake.Node[i].Nd_Dir     = LEFT;
        Snake.Node[i].Prty.Place = SIDE;
        Snake.Node[i].Prty.R_Dir = NOT_DEF;
    }
*/
    
    Snake.nNode = SNAKE_INIT_NODE;
    Snake.Sk_Dir = RIGHT;

    for (i = 0; i < SNAKE_INIT_NODE; ++i)
    {
        Snake.Node[i].X          = SNAKE_INIT_NODE - i - 1;
        Snake.Node[i].Y          = 4;
        Snake.Node[i].Nd_Dir     = RIGHT;
        Snake.Node[i].Prty.Place = SIDE;
        Snake.Node[i].Prty.R_Dir = NOT_DEF;
    }
}

#define VALID_IPT   0   // valid input
#define INVALID_IPT 1

#define SNAKE_DISABLE_SUICIDE

/*****************************************************************************
 * Function      : _judge_dir_validity
 * Description   : Judging input dir validity.
                   if next position of the first node coincide with other nodes, 
                   this input dir is invalid, 
                   because you can't kill yourself :)
 * Param         : None
 * Returns       : u8
 *
 * Record        :
 * 1.Date        : 10/13/2016
 *   Author      : Mr_Liang
 *   Modification: Created function
*****************************************************************************/
static u8 _judge_dir_validity(NODE_DIR skDir)
{
#if defined(SNAKE_DISABLE_SUICIDE) 

    u8 NodeCnt;
    u8 tmpX, tmpY;
    
    tmpX = Snake.Node[0].X;
    tmpY = Snake.Node[0].Y;

    switch (skDir)
    {
        case UP:    tmpY -= 1;
                    break;
        case RIGHT: tmpX += 1;
                    break;
        case DOWN:  tmpY += 1;
                    break;
        case LEFT:  tmpX -= 1;
                    break;
        default:
                    break;
    }
    
    for (NodeCnt = 0; NodeCnt < Snake.nNode; ++NodeCnt)
    {
        if (tmpX == Snake.Node[NodeCnt].X 
            && tmpY == Snake.Node[NodeCnt].Y)
        {
//            UartSendDebugInfo("\r\nRETURN INVALID, NODE BESIDE");
            return INVALID_IPT;
        }
    }

    if (tmpX > X_AXIS_MAX
        || tmpY > Y_AXIS_MAX)
    {
//        UartSendDebugInfo("\r\nRETURN INVALID, GO OUT OF BOUNDARY");
        return INVALID_IPT;
    }

//    UartSendDebugInfo("\r\nRETURN VALID");
    return VALID_IPT;

#endif /*#if defined(CANNOT_KILL_YOURSELF) */
}

/*****************************************************************************
 * Function      : _kv_judge_dir
 * Description   : judge rotate dirction and objective dir by button input 
 * Param         : void  
 * Returns       : none
 *
 * Record        :
 * 1.Date        : 10/07/2016
 *   Author      : Mr_Liang
 *   Modification: Created function
*****************************************************************************/
static void _kv_judge_dir(void)
{
    NODE_DIR SkDir;
    s8       RotDirVal;              // Signed, rotate dirction value
    bit      fKT = 0, fValidity = 0;
    
    switch (KeyValue) 
    {
        case KV_UP:                  // SNAKE move up
            KeyValue = KV_NULL; 
            SkDir = UP;
            fKT   = 1;
                break;
    
        case KV_RIGHT:               // SNAKE move right
            KeyValue = KV_NULL;    
            SkDir    = RIGHT;
            fKT      = 1;
                break;
                
        case KV_DOWN:                 // SNAKE move down
            KeyValue = KV_NULL;   
            SkDir    = DOWN;
            fKT      = 1;
                break;    

        case KV_LEFT:                // SNAKE move left
            KeyValue = KV_NULL;    
            SkDir    = LEFT;
            fKT      = 1;
                break;

        default:
        
                break;
    }

    if (fKT)
    {
        fKT = 0;

        if (_judge_dir_validity(SkDir) == INVALID_IPT)
        {
            return;
        }
        
        if (SkDir == Snake.Sk_Dir) {
//            UartSendDebugInfo("\r\nSame dir ");
            return;
        }

        RotDirVal = Snake.Sk_Dir -  SkDir;
        if (RotDirVal == 1 || RotDirVal == -3)          // rotate 90 degree clockwise
        {
            // Assigning value to the first node at here, 
            // because the value will assign to the second node later
            Snake.Node[0].Nd_Dir     = Snake.Sk_Dir;
            Snake.Node[0].Prty.Place = CORNER;
            Snake.Node[0].Prty.R_Dir = CW;
            
            Snake.Sk_Dir             = SkDir;
            
//            UartSendDebugInfo("\r\nClockwise : ");
//            UartSendDebugData(Snake.Sk_Dir, DEC);
        }
        else if (RotDirVal == -1 || RotDirVal == 3)   // rotate 90 degree counter clockwise
        {
            Snake.Node[0].Nd_Dir     = Snake.Sk_Dir;
            Snake.Node[0].Prty.Place = CORNER;
            Snake.Node[0].Prty.R_Dir = COUNTER_CW;
            
            Snake.Sk_Dir             = SkDir;
            
//            UartSendDebugInfo("\r\nCounter Clockwise ");
//            UartSendDebugData(Snake.Sk_Dir, DEC);
        }
        else
        {
//            UartSendDebugInfo("\r\nInvalid dir ");
        }
    }
}

#undef VALID_IPT
#undef INVALID_IPT

/*****************************************************************************
 * Function      : SnakeCalcCoor
 * Description   : calculate coordinate of each node
 * Param         : void  
 * Returns       : none
 *
 * Record        :
 * 1.Date        : 10/08/2016
 *   Author      : Mr_Liang     
 *   Modification: Created function
 *
 * 2.Date        : 10/13/2016
 *   Author      : Mr_Liang
 *   Modification: fixd bug when there are only two nodes,
                   the dir of the last node (tail) is wrong
*****************************************************************************/
static void SnakeCalcCoor(void)
{
    u8 i;
    
    _kv_judge_dir(); // judge botton input is valid or invalid.

    // get the coordinate of tail(the last node) before shift, 
    // use to clear shadow of tail
    SkTailCoorX = Snake.Node[Snake.nNode - 1].X;
    SkTailCoorY = Snake.Node[Snake.nNode - 1].Y;
    SkTailDir   = Snake.Node[Snake.nNode - 1].Nd_Dir;
    
    for (i = Snake.nNode - 1; i > 0; --i)
    {
        Snake.Node[i] = Snake.Node[i - 1];
    }
    // Tail(the last node) should always be SIDE.
    Snake.Node[Snake.nNode - 1].Prty.Place = SIDE;
    Snake.Node[Snake.nNode - 1].Prty.R_Dir = NOT_DEF;
    if (Snake.nNode > 2) {
        Snake.Node[Snake.nNode - 1].Nd_Dir = Snake.Node[Snake.nNode - 2].Nd_Dir;
    }
    // fixd bug when there are only two nodes, 
    // the dir of the last node (tail) is wrong
    else if (Snake.nNode == 2) {
        Snake.Node[Snake.nNode - 1].Nd_Dir = Snake.Sk_Dir;  
    }
    
    switch (Snake.Sk_Dir)       // handling the first node
    {
        case UP:    Snake.Node[0].Y -= 1;
                    break;
        case RIGHT: Snake.Node[0].X += 1;
                    break;
        case DOWN:  Snake.Node[0].Y += 1;
                    break;
        case LEFT:  Snake.Node[0].X -= 1;
                    break;
        default:
                    break;
    }
    Snake.Node[0].Nd_Dir = Snake.Sk_Dir;
    Snake.Node[0].Prty.Place = SIDE;
    Snake.Node[0].Prty.R_Dir = NOT_DEF;
    
}

/*****************************************************************************
 * Function      : _draw_node_side
 * Description   : Draw side node, 4 conditions
 * Param         : u8 x               
                   u8 y               
                   NODE_DIR dir       
                   PIXEL_STATE color  
 * Returns       : none
 *
 * Record        :
 * 1.Date        : 10/13/2016
 *   Author      : Mr_Liang
 *   Modification: Created function
*****************************************************************************/
static void _draw_node_side(u8 x, u8 y, NODE_DIR dir, PIXEL_STATE color)
{
    u8 offsetX, offsetY;

    x = (x << 2) + 2;
    y = (y << 2) + 10;
    
    for (offsetX = 0; offsetX < 4; ++offsetX) {
        for (offsetY = 0; offsetY < 4; ++offsetY) {
            if (color == ON) {
                LcdDrawPoint(x + offsetX, 
                             y + offsetY, 
                             (Node_Side[dir - 1][offsetX] >> offsetY) & 0x01);
            }
            else if (color == OFF) {
                LcdDrawPoint(x + offsetX, y + offsetY, 0);
            }
        }
    }
}

/*****************************************************************************
 * Function      : _draw_node_corner
 * Description   : Draw corner node, 8 conditions
 * Param         : u8 x           
                   u8 y           
                   NODE_DIR dir   
                   ROT_DIR rotDir  
 * Returns       : none
 *
 * Record        :
 * 1.Date        : 10/07/2016
 *   Author      : Mr_Liang
 *   Modification: Created function
*****************************************************************************/
static void _draw_node_corner(u8 x, u8 y, NODE_DIR dir, ROT_DIR rotDir)
{
    u8 offsetX, offsetY;

    x = (x << 2) + 2;
    y = (y << 2) + 10;

    if (dir == UP || dir == DOWN)
    {
        for (offsetX = 0; offsetX < 4; ++offsetX) 
        {
            for (offsetY = 0; offsetY < 4; ++offsetY) 
            {
                if (rotDir == CW)      // Clockwise
                {
                    LcdDrawPoint(x + offsetX, 
                                 y + offsetY, 
                                 (Node_Corner[dir - 1][offsetX] >> offsetY) & 0x01);
                }
                else if (rotDir == COUNTER_CW)      // Counter ClockWise
                {
/*                  // mirror up and down, up side down, 上下镜像
                    LcdDrawPoint(x + offsetX, y + offsetY, 
                                ((Node_Corner[dir - 1][offsetX] << offsetY) & 0x08) >> 3);
*/
                    // mirror left and right, 左右镜像  
                    LcdDrawPoint(x + offsetX, 
                                 y + offsetY, 
                                 (Node_Corner[dir - 1][3 - offsetX] >> offsetY) & 0x01);
                }
            }
        }
    }

    if (dir == LEFT || dir == RIGHT)
    {
        for (offsetX = 0; offsetX < 4; ++offsetX) 
        {
            for (offsetY = 0; offsetY < 4; ++offsetY) 
            {
                if (rotDir == CW)       // Clockwise
                {
                    LcdDrawPoint(x + offsetX, 
                                 y + offsetY, 
                                 (Node_Corner[dir - 1][offsetX] >> offsetY) & 0x01);
                }
                else if (rotDir == COUNTER_CW)  // Counter Clockwise
                {
                    // mirror up and down, up side down, 上下镜像
                    LcdDrawPoint(x + offsetX, 
                                 y + offsetY,       
                                 ((Node_Corner[dir - 1][offsetX] << offsetY) & 0x08) >> 3);
/*                  // mirror left and right, 左右镜像  
                    if (color == ON) {
                        LcdDrawPoint(x + offsetX, y + offsetY, 
                                    (Node_Corner[dir - 1][3 - offsetX] >> offsetY) & 0x01);
                    }
*/
                }
            }
        }
    }
}

/*****************************************************************************
 * Function      : _draw_snake
 * Description   : Draw entire snake
 * Param         : void  
 * Returns       : static
 *
 * Record        :
 * 1.Date        : 10/14/2016
 *   Author      : Mr_Liang
 *   Modification: Created function
*****************************************************************************/
static void _draw_snake(void)
{
    u8 NodeCnt;
    
    for (NodeCnt = 0; NodeCnt < Snake.nNode; ++NodeCnt) 
    {
        if (Snake.Node[NodeCnt].Prty.Place == SIDE)
        {
            _draw_node_side(Snake.Node[NodeCnt].X, 
                            Snake.Node[NodeCnt].Y, 
                            Snake.Node[NodeCnt].Nd_Dir,
                            ON);
        }
        else if (Snake.Node[NodeCnt].Prty.Place == CORNER)
        {
            _draw_node_corner(Snake.Node[NodeCnt].X, 
                              Snake.Node[NodeCnt].Y, 
                              Snake.Node[NodeCnt].Nd_Dir, 
                              Snake.Node[NodeCnt].Prty.R_Dir);
        }
    }
}

/*****************************************************************************
 * Function      : _draw_node
 * Description   : Draw nodes
 * Param         : void  
 * Returns       : none
 *
 * Record        :
 * 1.Date        : 10/08/2016
 *   Author      : Mr_Liang
 *   Modification: VERSION 1.0
                   Created function.
 *
 * 2.Date        : 10/12/2016
 *   Author      : Mr_Liang
 *   Modification: VERSION 2.0
                   Higher speed algorithm, but can't disp corner node.
 *
 * 3.Date        : 10/13/2016
 *   Author      : Mr_Liang
 *   Modification: VERSION 2.5
                   Optimize algorithm, fixed disp corner node problem.
 *
 * 4.Date        : 10/14/2016
 *   Author      : Mr_Liang
 *   Modification: VERSION 3.0
                   clear disp of the last node which is in old position.
*****************************************************************************/
static void _draw_node(void) 
{   
// VERSION 3.0
    if (fClrTail == TRUE)
    {
        fClrTail = FALSE;
        // clear disp of the last node which is in the old position.
        _draw_node_side(SkTailCoorX, SkTailCoorY, SkTailDir, OFF); 
    }
    
    _draw_node_side(Snake.Node[Snake.nNode - 1].X, 
                    Snake.Node[Snake.nNode - 1].Y, 
                    Snake.Node[Snake.nNode - 1].Nd_Dir,
                    ON);

    // if the second node is corner than disp it
    if (Snake.Node[1].Prty.Place == CORNER)
    {
        _draw_node_corner(Snake.Node[1].X, 
                          Snake.Node[1].Y, 
                          Snake.Node[1].Nd_Dir, 
                          Snake.Node[1].Prty.R_Dir);
    }
    
    // disp head
    _draw_node_side(Snake.Node[0].X,
                    Snake.Node[0].Y,
                    Snake.Node[0].Nd_Dir,
                    ON);

/*
// VERSION 2.5
    u8 NodeCnt;
    
    for (NodeCnt = 0; NodeCnt < Snake.nNode; ++NodeCnt)
    {
        // fixed orientation problem of the last node 
        if (NodeCnt == Snake.nNode - 1) 
        {
            _draw_node_side(Snake.Node[Snake.nNode - 1].X, 
                            Snake.Node[Snake.nNode - 1].Y, 
                            Snake.Node[Snake.nNode - 1].Nd_Dir,
                            ON);
        }
        
        if (Snake.Node[NodeCnt].Prty.Place == CORNER)
        {
            _draw_node_corner(Snake.Node[NodeCnt].X, 
                              Snake.Node[NodeCnt].Y, 
                              Snake.Node[NodeCnt].Nd_Dir, 
                              Snake.Node[NodeCnt].Prty.R_Dir);
        }
    }
    
    _draw_node_side(SkTailCoorX, SkTailCoorY, SkTailDir, OFF);  
    _draw_node_side(Snake.Node[0].X,
                    Snake.Node[0].Y,
                    Snake.Node[0].Nd_Dir,
                    ON);
*/
/*
// VERSION 2.0
// higher speed algorithm, but cannot disp corner node.
    _draw_node_side(SkTailCoorX, SkTailCoorY, SkTailDir, OFF);  
    _draw_node_side(Snake.Node[0].X,
                    Snake.Node[0].Y,
                    Snake.Node[0].Nd_Dir,
                    ON);
*/
    
/*  
// VERSION 1.0
    u8 NodeCnt;

    // clear shadow of tail
    _draw_node_side(SkTailCoorX, SkTailCoorY, SkTailDir, OFF);
    
    for (NodeCnt = 0; NodeCnt < Snake.nNode; ++NodeCnt) 
    {
        if (Snake.Node[NodeCnt].Prty.Place == SIDE)
        {
            _draw_node_side(Snake.Node[NodeCnt].X, 
                            Snake.Node[NodeCnt].Y, 
                            Snake.Node[NodeCnt].Nd_Dir,
                            ON);
        }
        else if (Snake.Node[NodeCnt].Prty.Place == CORNER)
        {
            _draw_node_corner(Snake.Node[NodeCnt].X, 
                              Snake.Node[NodeCnt].Y, 
                              Snake.Node[NodeCnt].Nd_Dir, 
                              Snake.Node[NodeCnt].Prty.R_Dir);
        }
    }
*/
}

/*****************************************************************************
 * Function      : SnakeDisp
 * Description   : Disp Snake
 * Param         : void  
 * Returns       : none
 *
 * Record        :
 * 1.Date        : 10/08/2016
 *   Author      : Mr_Liang
 *   Modification: Created function
*****************************************************************************/
static void SnakeDisp(void)
{    
    if (Snake.nNode > SNAKE_NODE_MAX)
    {
        return;
    }

    _draw_node();

//_draw_node_side(SkTailCoorX, SkTailCoorY, SkTailDir, OFF); 
//_draw_snake();

    LcdWrCmd(EXT_GRAPH_ON);
}

/*****************************************************************************
 * Function      : JudgeConicide
 * Description   : Judging head coordinate whether conicide with other nodes or wall
 * Param         : void  
 * Returns       : static
 *
 * Record        :
 * 1.Date        : 10/14/2016
 *   Author      : Mr_Liang
 *   Modification: Created function
 *
 * 2.Date        : 10/17/2016
 *   Author      : Mr_Liang
 *   Modification: Judging head coincide with body start from the seventh node.
*****************************************************************************/
static void JudgeCollision(void)
{
    bit fRST = 0;       // Reset flag
    u8 NodeCnt;

    // Judging wheather head node coincides with other nodes or not.
    // Collision censor starts from the seventh node, because collision 
    // could only happen with at least seven nodes.
    for (NodeCnt = 6; NodeCnt < Snake.nNode; ++NodeCnt)
    {
        if (Snake.Node[0].X == Snake.Node[NodeCnt].X 
            && Snake.Node[0].Y == Snake.Node[NodeCnt].Y)
        {
            fRST = TRUE;
            break;
        }
    }

    // if snake go out of boundary
    if (Snake.Node[0].X > X_AXIS_MAX
        || Snake.Node[0].Y > Y_AXIS_MAX)
    {
        fRST = TRUE;
    }

    if (fRST)
    {
        Delay_Xms(100);
        SnakeParamInit();
        ClrCanvas();
        for (NodeCnt = 0; NodeCnt < FOOD_MAX; ++NodeCnt)
        {
            Food[NodeCnt].State = IDLE;
        }
        LcdDrawStrNum(30, 0, 3, GmScore, OFF);
        LcdDrawStrNum(30, 0, 3, 0, ON);
//        GmScore = 0;
    }
}

/*****************************************************************************
 * Function      : JudgeGrowth
 * Description   : snake grows when the head coincide with food
 * Param         : void  
 * Returns       : static
 *
 * Record        :
 * 1.Date        : 10/15/2016
 *   Author      : Mr_Liang
 *   Modification: Created function
*****************************************************************************/
static u8 JudgeGrowth(void)
{
    u8 i;
    
    for (i = 0; i < FOOD_MAX; ++i)
    {
        if (Snake.Node[0].X == Food[i].X 
            && Snake.Node[0].Y == Food[i].Y
            && Food[i].State == OCCUPY)
        {
            Food[i].State = IDLE;

            Snake.nNode += 1;
            Snake.Node[Snake.nNode - 1].X      = SkTailCoorX;
            Snake.Node[Snake.nNode - 1].Y      = SkTailCoorY;
            Snake.Node[Snake.nNode - 1].Nd_Dir = SkTailDir;
            fClrTail = FALSE;
            
            UartSendDebugInfo("\r\nSNAKE GROW ");
            UartSendDebugData(Food[i].X, DEC);
            UartSendDebugData(Food[i].Y, DEC);
            UartSendDebugInfo(", nNode = ");
            UartSendDebugData(Snake.nNode, DEC);

            return 1;
        }
    }

    fClrTail = TRUE;    // if snake do not eat food, clear disp of the last node
    return 0;
}
 
void SnakeGame_Main(void)
{
    bit fGrowth;
    u8 i;
    u8 xdata Hertz = 0;
    
    pfCurrentFunc = &SnakeGame_Main;

    if (fKeyTrig) 
    {
        fKeyTrig = FALSE;

        SetMenuFlag(FALSE);

        LcdWrCmd(EN_BAS_INSTRUCTION);
        LcdWrCmd(CLR_DDRAM_INIT_AC);
        LcdWrCmd(EN_EXT_INSTRUCTION);
        LcdClrGDRAM();

        SnakeParamInit();
        for (i = 0; i < FOOD_MAX; ++i)
        {
            Food[i].State = IDLE;
        }
        DrawBoundary();
        LcdDrawStrNum(30, 0, 3, 0, ON); // Draw Init Score
        _draw_snake();
    }

    FreqCnt = 0;

    SnakeCalcCoor();

    FoodDisp();
    fGrowth = JudgeGrowth();
    if (fGrowth)
    {
        DataDisp();
    }
    JudgeCollision();
    SnakeDisp();

//    Delay_Xms(300);g33
    Delay_Xms(10);

    Hertz = 1000 / FreqCnt;
    UartSendDebugInfo("\r\n");
    UartSendDebugData(Hertz, DEC);  
    UartSendDebugInfo("Hz");

    if (KeyValue == KV_ESC) 
    {
        KeyValue = KV_NULL;     // 清除等待时按下的键值

        LcdWrCmd(EN_BAS_INSTRUCTION);
        SetMenuFlag(TRUE);
        fMenuParamInit = FALSE;       // 防止初始化菜单参数, 使光标停留在原地
        fCursorInit    = FALSE;
 
        Menu_Games();
    }
}
  
/*    
// Opt data of each node 
    for (NodeCnt = 0; NodeCnt < Snake.nNode; ++NodeCnt)
    {
        UartSendDebugInfo("\r\nNode ");
        UartSendDebugData(NodeCnt, DEC);
        UartSendDebugInfo(", X ");
        UartSendDebugData(Snake.Node[NodeCnt].X, DEC);
        UartSendDebugInfo(", Y ");
        UartSendDebugData(Snake.Node[NodeCnt].Y, DEC);
        UartSendDebugInfo(", prty:");
        
        if (Snake.Node[NodeCnt].Prty.Place == SIDE) {
            UartSendDebugInfo(" SIDE   ");
        }
        else {
            UartSendDebugInfo(" CORNER ");
        }
        UartSendDebugData(Snake.Node[NodeCnt].Nd_Dir, DEC);
        UartSendDebugData(Snake.Node[NodeCnt].Prty.R_Dir, DEC);
    }
    UartSendDebugInfo("\r\n");
*/

/*****************************************************************************
 * Function      : RandFuncTest
 * Description   : generate 20000 random nums from 0 to 4, 
                   disp quantity on the screen
 * Param         : void  
 * Returns       : none
 *
 * Record        :
 * 1.Date        : 09/19/2016
 *   Author      : Mr_Liang
 *   Modification: Created function
*****************************************************************************/
/*
void RandFuncTest(void)
{
    u16 i;
    u16 xdata arr[5] = {0};

    pfCurrentFunc = &RandFuncTest; 

    if (fKeyTrig) {
        fKeyTrig = FALSE;

        SetMenuFlag(FALSE);

        LcdWrCmd(EN_BAS_INSTRUCTION);
        LcdWrCmd(CLR_DDRAM_INIT_AC);
        LcdWrCmd(EN_EXT_INSTRUCTION);
        LcdClrGDRAM();
    }

    if (KeyValue == KV_ENTER) {
        KeyValue = KV_NULL;
        
        srand(TH0 + TL0);   
        
        for (i = 0; i < 20000; i++) {
            arr[(rand() % 5)] += 1;
        }

        // Output on screen
        LcdWrCmd(EN_EXT_INSTRUCTION);
        for (i = 0; i < 5; i++) {
            LcdDrawStrNum(10, 10 + 10 * i, 4, arr[i], ON);
        }
        LcdWrCmd(EXT_GRAPH_ON);
    }
    
    if (KeyValue == KV_ESC) {
        KeyValue = KV_NULL;     // 清除等待时按下的键值
        
        SetMenuFlag(TRUE);
        fMenuParamInit = FALSE;       // 防止初始化菜单参数, 使光标停留在原地
        fCursorInit    = FALSE;

        Menu_Games();
    }
}
*/

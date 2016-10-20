/*****************************************************************************
 * File Name     : Snake.h
 * Author        : Mr_Liang
 * Date          : 2016/10/05
 * Description   : Snake.c header file
 * Version       : 1.0
 *
 * Record        :
 * 1.Date        : 2016/10/05
 *   Author      : Mr_Liang
 *   Modification: Created file
*****************************************************************************/

#ifndef __GAME_SNAKE_H__
#define __GAME_SNAKE_H__

//void RandFuncTest(void);
void SnakeGame_Main(void);

extern bit fNewFood;
extern volatile u16 xdata GmScore;
extern volatile u16 xdata FreqCnt;

#endif /* __SNAKE_H__ */

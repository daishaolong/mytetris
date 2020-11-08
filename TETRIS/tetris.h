#ifndef _TETRIS_H
#define _TETRIS_H
#include "sys.h"


void Tetris_Init(void);
void Tetris_Tmr_Callback(void *ptmr, void *parg);
void Turn_90(void);
void Move_Rigth(void);
void Move_Left(void);
void Move_Down(void);
void Show_Game_Board(void);
#endif  /*_TETRIS_H*/

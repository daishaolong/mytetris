#ifndef _TETRIS_H
#define _TETRIS_H
#include "sys.h"


//����С�����С
#define SHAPE_ROW_SIZE      4//����ÿ��С������Ŀ
#define SHAPE_COL_SIZE      4//����ÿ��С������Ŀ

//��ͼ�к���
#define MAP_COL_SIZE     14//
#define MAP_ROW_SIZE     28//

#define LEFT_WALL_SIZE    2
#define RIGHT_WALL_SIZE   2
#define DOWN_WALL_SIZE    2
#define UP_WALL_SIZE      SHAPE_ROW_SIZE

typedef struct
{
    u16 row;//������
    u16 col;//������
    u8 index;//��״����ֵ
    u8 dir;//��״����ֵ
} block_t;
//��״Ԫ��:4��4��
typedef struct
{
    u8 row0 : 4;
    u8 row1 : 4;
    u8 row2 : 4;
    u8 row3 : 4;
} element_t;

//��״������
typedef union
{
    element_t ele;
    u16 val;
} shape_t;

//7�ֻ����ṹ��ÿ����4����ת����
extern const shape_t shape_array[7][4];
typedef enum
{
	SHAPE_0=0,
	SHAPE_1,
	SHAPE_2,
	SHAPE_3,
	SHAPE_4,
	SHAPE_5,
	SHAPE_6,
	NONE,
	WALL,
	ELE_MAX
}map_elemnt_t;//��ͼ�д�ŵ�Ԫ��
//��ͼ
extern u8 map[MAP_ROW_SIZE][MAP_COL_SIZE];//15*12
extern block_t cur_block;//��ǰ��
extern block_t next_block;//��һ����
extern u16 kill_lines;
extern u32 run_time;
extern u8 game_over;
extern u8 game_level;
void Tetris_Init(void);
void Tetris_Tmr_Callback(void *ptmr, void *parg);
void Turn_90(void);
void Move_Rigth(void);
void Move_Left(void);
u8 Move_Down(void);
void Move_Bottom(void);
#endif  /*_TETRIS_H*/

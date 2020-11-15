#ifndef _TETRIS_H
#define _TETRIS_H
#include "sys.h"


//基本小方块大小
#define SHAPE_ROW_SIZE      4//方块每行小方块数目
#define SHAPE_COL_SIZE      4//方块每列小方块数目

//地图行和列
#define MAP_COL_SIZE     14//
#define MAP_ROW_SIZE     28//

#define LEFT_WALL_SIZE    2
#define RIGHT_WALL_SIZE   2
#define DOWN_WALL_SIZE    2
#define UP_WALL_SIZE      SHAPE_ROW_SIZE

typedef struct
{
    u16 row;//列坐标
    u16 col;//行坐标
    u8 index;//形状索引值
    u8 dir;//形状方向值
} block_t;
//形状元素:4行4列
typedef struct
{
    u8 row0 : 4;
    u8 row1 : 4;
    u8 row2 : 4;
    u8 row3 : 4;
} element_t;

//形状联合体
typedef union
{
    element_t ele;
    u16 val;
} shape_t;

//7种基本结构，每种有4个旋转方向
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
}map_elemnt_t;//地图中存放的元素
//地图
extern u8 map[MAP_ROW_SIZE][MAP_COL_SIZE];//15*12
extern block_t cur_block;//当前块
extern block_t next_block;//下一个块
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

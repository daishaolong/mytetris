#include "game_board.h"
#include "tetris.h"
#include  "lcd.h"
#include  "delay.h"
#include  <string.h>
#include  <stdio.h>


//基本小方块大小
#define BASE_BLOCK_PIXEL_SIZE     15//30// 方块移动的基本单位
//地图使用的行和列的像素点
#define MAP_ROW_PIXEL_MAX       SSD_HOR_RESOLUTION//<=SSD_HOR_RESOLUTION//(SSD_HOR_RESOLUTION/BASE_BLOCK_SIZE - SHAPE_ROW_SIZE)
#define MAP_COL_PXIEL_MAX       SSD_VER_RESOLUTION//480
const u16  map_elemnt_color[ELE_MAX] =
{
    CYAN,   //蓝绿色
    MAGENTA,//品红
    GREEN,  //绿色
    YELLOW, //黄色
    BROWN,  //棕色
    BRRED,  //棕红色
    GRAY,   //灰色
    WHITE,//背景色
    BLACK//围墙色
};





//<row,col>位置画基本小方块方块
void draw_block(u16 row, u16 col, u16 color)
{
    row *= BASE_BLOCK_PIXEL_SIZE; //
    col *= BASE_BLOCK_PIXEL_SIZE; //
    LCD_Fill(col, row, col + BASE_BLOCK_PIXEL_SIZE, row + BASE_BLOCK_PIXEL_SIZE, color);
    if (map_elemnt_color[NONE] != color)
    {
        LCD_DrawRectangle(col,  row,  col + BASE_BLOCK_PIXEL_SIZE, row + BASE_BLOCK_PIXEL_SIZE);//不是背景色则画框
    }

}
#define  ROW  (UP_WALL_SIZE)
#define  COL  (MAP_COL_SIZE+2)
void Show_Next_Shape(void)
{

	u16 val= shape_array[next_block.index][next_block.dir].val;
	u16  color;
    u8 row,col;
	u16 bit;
	color=map_elemnt_color[NONE];
	for(row=0;row<SHAPE_ROW_SIZE;row++)
	{
		bit=0x0008;
		for(col=0;col<SHAPE_COL_SIZE;col++)
		{
			if (!(val&bit))
			{
				draw_block(ROW+row,COL+col+1,color);
			}
			
			bit>>=1;
		}
		val>>=4;
	}
	val= shape_array[next_block.index][next_block.dir].val;
	color=map_elemnt_color[next_block.index];
	for(row=0;row<SHAPE_ROW_SIZE;row++)
	{
		bit=0x0008;
		for(col=0;col<SHAPE_COL_SIZE;col++)
		{
			if (val&bit)
			{
				draw_block(ROW+row,COL+col+1,color);
			}
			bit>>=1;
		}
		val>>=4;
	}	
}
void Show_Cur_Shape(void)
{

	u16 val= shape_array[cur_block.index][cur_block.dir].val;
	u16  color;
    u8 row,col;
	u16 bit;
	color=map_elemnt_color[NONE];
	for(row=0;row<SHAPE_ROW_SIZE;row++)
	{
		bit=0x0008;
		for(col=0;col<SHAPE_COL_SIZE;col++)
		{
			if (!(val&bit))
			{
				draw_block(ROW+row,COL+col,color);
			}
			
			bit>>=1;
		}
		val>>=4;
	}
	val= shape_array[cur_block.index][cur_block.dir].val;
	color=map_elemnt_color[cur_block.index];
	for(row=0;row<SHAPE_ROW_SIZE;row++)
	{
		bit=0x0008;
		for(col=0;col<SHAPE_COL_SIZE;col++)
		{
			if (val&bit)
			{
				draw_block(ROW+row,COL+col,color);
			}
			bit>>=1;
		}
		val>>=4;
	}	
}
/*
void LCD_ShowNum(u16 x,u16 y,u32 num,u8 len,u8 size);  						//显示一个数字
void LCD_ShowxNum(u16 x,u16 y,u32 num,u8 len,u8 size,u8 mode);				//显示 数字
void LCD_ShowString(u16 x,u16 y,u16 width,u16 height,u8 size,u8 *p);
*/
void Show_Game_Over(void)
{
	char info[50];
	POINT_COLOR=RED;
	snprintf(info,sizeof(info),"game over!!!!");	
	LCD_ShowString( COL*BASE_BLOCK_PIXEL_SIZE, (ROW+SHAPE_ROW_SIZE+10)*BASE_BLOCK_PIXEL_SIZE, 
					200, 16, 16,(u8*)info);	
	POINT_COLOR=BLACK;		
}
void Show_Score(void)
{
	
	char info[50];
	POINT_COLOR=RED;
	snprintf(info,sizeof(info),"kill lines:%d",kill_lines);
	LCD_ShowString( COL*BASE_BLOCK_PIXEL_SIZE, (ROW+SHAPE_ROW_SIZE+2)*BASE_BLOCK_PIXEL_SIZE, 
					200, 16, 16,(u8*)info);
	snprintf(info,sizeof(info),"game time:");
	LCD_ShowString( COL*BASE_BLOCK_PIXEL_SIZE, (ROW+SHAPE_ROW_SIZE+4)*BASE_BLOCK_PIXEL_SIZE, 
					200, 16, 16,(u8*)info);	
	u16 min,second,ms;
//	u32 time_ms=run_time;
	min=run_time/(60*1000);
	second=(run_time%(60*1000))/1000;
	ms=(run_time%(60*1000))%1000;
	snprintf(info,sizeof(info),"%2d min %2d s %3d ms",min,second,ms);
	LCD_ShowString( COL*BASE_BLOCK_PIXEL_SIZE, (ROW+SHAPE_ROW_SIZE+6)*BASE_BLOCK_PIXEL_SIZE, 
					200, 16, 16,(u8*)info);	
	snprintf(info,sizeof(info),"game level:%d",game_level);	
	LCD_ShowString( COL*BASE_BLOCK_PIXEL_SIZE, (ROW+SHAPE_ROW_SIZE+8)*BASE_BLOCK_PIXEL_SIZE, 
					200, 16, 16,(u8*)info);	
	POINT_COLOR=BLACK;	
}
void Show_Map(void)
{
	//UP_WALL_SIZE 上面4行用来放方块，不显示出来
    u16 row, col;
    for (row = UP_WALL_SIZE; row < MAP_ROW_SIZE - DOWN_WALL_SIZE; row++) //SHAPE_ROW_SIZE
    {
        for (col = LEFT_WALL_SIZE; col < MAP_COL_SIZE - RIGHT_WALL_SIZE; col++)
        {
            if (NONE == map[row][col])
            {
                draw_block(row, col, map_elemnt_color[NONE]);
            }
        }
    }
	//UP_WALL_SIZE 上面4个不显示，用于放置方块
    for (row = UP_WALL_SIZE; row < MAP_ROW_SIZE - DOWN_WALL_SIZE + 1; row++) //SHAPE_ROW_SIZE
    {
        for (col = LEFT_WALL_SIZE - 1; col < MAP_COL_SIZE - RIGHT_WALL_SIZE + 1; col++)
        {
            if (NONE != map[row][col])
            {
                draw_block(row, col, map_elemnt_color[map[row][col]]);
            }
        }
    }
//  //--TODO
//  LCD_DrawLine(0,SHAPE_ROW_SIZE*BASE_BLOCK_PIXEL_SIZE,
//              MAP_COL_SIZE*BASE_BLOCK_PIXEL_SIZE,SHAPE_ROW_SIZE*BASE_BLOCK_PIXEL_SIZE);
//    for (col = LEFT_WALL_SIZE - 1; col < MAP_COL_SIZE - RIGHT_WALL_SIZE + 1; col++)
//    {
//        draw_block(UP_WALL_SIZE - 1,  col,  map_elemnt_color[WALL]);
//    }	
}
void Show_Game_Board(void)
{
	Show_Map();
	Show_Next_Shape();
//	Show_Cur_Shape();
	Show_Score();
	if(1==game_over)
	{
		Show_Game_Over();
	}
}


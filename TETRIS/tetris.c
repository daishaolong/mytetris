#include "tetris.h"
#include  "lcd.h"
#include  "delay.h"
#include "usart.h"
#include  <string.h>
#include  <stdlib.h>
#if SYSTEM_SUPPORT_OS
    #include "includes.h"                   //ucos 使用
#endif

#define TETRIS_BACK_COLOR    WHITE//背景色
#define TETRIS_WALL_COLOR    BLACK//围墙色
//基本小方块大小
#define BASE_BLOCK_SIZE     20// 方块移动的基本单位
#define SHAPE_ROW_SIZE      4//方块每行小方块数目
#define SHAPE_COL_SIZE      4//方块每列小方块数目

//地图使用的行和列的像素点
#define MAP_ROW_PIXEL_MAX       600//<=SSD_HOR_RESOLUTION
#define MAP_COL_PXIEL_MAX       SSD_VER_RESOLUTION//480

//地图行和列
#define MAP_COL_SIZE     (MAP_COL_PXIEL_MAX/BASE_BLOCK_SIZE)//15
#define MAP_ROW_SIZE     (MAP_ROW_PIXEL_MAX/BASE_BLOCK_SIZE)//12

//地图
u16 map[MAP_ROW_SIZE][MAP_COL_SIZE];//15*12
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


const u16  base_shape_color[7] =
{
    CYAN,   //蓝绿色
    MAGENTA,//品红
    GREEN,  //绿色
    YELLOW, //黄色
    BROWN,  //棕色
    BRRED,  //棕红色
    GRAY,   //灰色
};
//7种基本结构，每种有4个旋转方向
const shape_t shape_array[7][4] =
{
    //0
    {
        {
            .ele.row0 = 0x0, //0000
            .ele.row1 = 0x6, //0110
            .ele.row2 = 0x6, //0110
            .ele.row3 = 0x0, //0000
        },
        {
            .ele.row0 = 0x0, //0000
            .ele.row1 = 0x6, //0110
            .ele.row2 = 0x6, //0110
            .ele.row3 = 0x0, //0000
        },
        {
            .ele.row0 = 0x0, //0000
            .ele.row1 = 0x6, //0110
            .ele.row2 = 0x6, //0110
            .ele.row3 = 0x0, //0000
        },
        {
            .ele.row0 = 0x0, //0000
            .ele.row1 = 0x6, //0110
            .ele.row2 = 0x6, //0110
            .ele.row3 = 0x0, //0000
        },
    },
    //1
    {
        {
            .ele.row0 = 0x4, //0100
            .ele.row1 = 0x4, //0100
            .ele.row2 = 0x4, //0100
            .ele.row3 = 0x4, //0100
        },
        {
            .ele.row0 = 0x0, //0000
            .ele.row1 = 0xf, //1111
            .ele.row2 = 0x0, //0000
            .ele.row3 = 0x0, //0000
        },
        {
            .ele.row0 = 0x4, //0100
            .ele.row1 = 0x4, //0100
            .ele.row2 = 0x4, //0100
            .ele.row3 = 0x4, //0100
        },
        {
            .ele.row0 = 0x0, //0000
            .ele.row1 = 0xf, //1111
            .ele.row2 = 0x0, //0000
            .ele.row3 = 0x0, //0000
        },
    },
    //2
    {
        {
            .ele.row0 = 0x0, //0000
            .ele.row1 = 0x4, //0100
            .ele.row2 = 0xe, //1110
            .ele.row3 = 0x0, //0000
        },
        {
            .ele.row0 = 0x0, //0000
            .ele.row1 = 0x4, //0100
            .ele.row2 = 0x6, //0110
            .ele.row3 = 0x4, //0100
        },
        {
            .ele.row0 = 0x0, //0000
            .ele.row1 = 0x0, //0000
            .ele.row2 = 0xe, //1110
            .ele.row3 = 0x4, //0100
        },
        {
            .ele.row0 = 0x0, //0000
            .ele.row1 = 0x4, //0100
            .ele.row2 = 0xc, //1100
            .ele.row3 = 0x4, //0100
        },
    },
    //3
    {
        {
            .ele.row0 = 0x6, //0110
            .ele.row1 = 0x2, //0010
            .ele.row2 = 0x2, //0010
            .ele.row3 = 0x0, //0000
        },
        {
            .ele.row0 = 0x2, //0010
            .ele.row1 = 0xe, //1110
            .ele.row2 = 0x0, //0000
            .ele.row3 = 0x0, //0000
        },
        {
            .ele.row0 = 0x4, //0100
            .ele.row1 = 0x4, //0100
            .ele.row2 = 0x6, //0110
            .ele.row3 = 0x0, //0000
        },
        {
            .ele.row0 = 0x0, //0000
            .ele.row1 = 0xe, //1110
            .ele.row2 = 0x8, //1000
            .ele.row3 = 0x0, //0000
        },
    },
    //4
    {
        {
            .ele.row0 = 0x6, //0110
            .ele.row1 = 0x4, //0100
            .ele.row2 = 0x4, //0100
            .ele.row3 = 0x0, //0000
        },
        {
            .ele.row0 = 0x0, //0000
            .ele.row1 = 0xe, //1110
            .ele.row2 = 0x2, //0010
            .ele.row3 = 0x0, //0000
        },
        {
            .ele.row0 = 0x4, //0100
            .ele.row1 = 0x4, //0100
            .ele.row2 = 0xc, //1100
            .ele.row3 = 0x0, //0000
        },
        {
            .ele.row0 = 0x8, //1000
            .ele.row1 = 0xe, //1110
            .ele.row2 = 0x0, //0000
            .ele.row3 = 0x0, //0000
        },
    },
    //5
    {
        {
            .ele.row0 = 0x0, //0000
            .ele.row1 = 0xc, //1100
            .ele.row2 = 0x6, //0110
            .ele.row3 = 0x0, //0000
        },
        {
            .ele.row0 = 0x0, //0000
            .ele.row1 = 0x2, //0010
            .ele.row2 = 0x6, //0110
            .ele.row3 = 0x4, //0100
        },
        {
            .ele.row0 = 0x0, //0000
            .ele.row1 = 0xc, //1100
            .ele.row2 = 0x6, //0110
            .ele.row3 = 0x0, //0000
        },
        {
            .ele.row0 = 0x0, //0000
            .ele.row1 = 0x2, //0010
            .ele.row2 = 0x6, //0110
            .ele.row3 = 0x4, //0100
        },
    },
    //6
    {
        {
            .ele.row0 = 0x0, //0000
            .ele.row1 = 0x6, //0011
            .ele.row2 = 0xc, //0110
            .ele.row3 = 0x0, //0000
        },
        {
            .ele.row0 = 0x0, //0000
            .ele.row1 = 0x4, //0100
            .ele.row2 = 0x6, //0110
            .ele.row3 = 0x2, //0010
        },
        {
            .ele.row0 = 0x0, //0000
            .ele.row1 = 0x6, //0011
            .ele.row2 = 0xc, //0110
            .ele.row3 = 0x0, //0000
        },
        {
            .ele.row0 = 0x0, //0000
            .ele.row1 = 0x4, //0100
            .ele.row2 = 0x6, //0110
            .ele.row3 = 0x2, //0010
        },
    },
};


void draw_shape(u16 row, u16 col, u8 shape_index, u8 dir)
{
    u8  j;
    u16 bit = 0x0008;
    u16 x1, y1 ;
    u16 val;
    u16 color;
    row *= BASE_BLOCK_SIZE; //
    col *= BASE_BLOCK_SIZE; //
    x1 = row;
    y1 = col;
    val = shape_array[shape_index][dir].val;
    color = base_shape_color[shape_index];
    while (val > 0)
    {
        for (j = 0; j < 4; j++)
        {
            if (val & bit)
            {
                LCD_Fill(x1, y1, x1 + BASE_BLOCK_SIZE, y1 + BASE_BLOCK_SIZE, color);
                LCD_DrawRectangle(x1,  y1,  x1 + BASE_BLOCK_SIZE, y1 + BASE_BLOCK_SIZE);
            }
            x1 += BASE_BLOCK_SIZE;
            bit >>= 1;
        }
        val >>= 4;
        bit = 0x0008;
        col += BASE_BLOCK_SIZE;
        x1 = row;
        y1 = col;
    }
}
void show_all_shape(void)
{
    u8 dir, index, row, col, startx;
    startx = 10;
    row = startx;
    col = 10;
    for (index = 0; index < 7; index++)
    {
        for (dir = 0; dir < 4; dir++)
        {
            draw_shape(row, col, index, dir);
            row += (SHAPE_COL_SIZE + 1);
        }
        row = startx;
        col += (SHAPE_ROW_SIZE + 1);
    }
}


typedef struct
{
    u16 row;//列坐标
    u16 col;//行坐标
    u8 index;//形状索引值
    u8 dir;//形状方向值
} block_t;

block_t cur_block;//当前块
block_t next_block;//下一个块

//清除<row,col>位置方块在地图上的标记
void Clear_Block_Flags(u16 row, u16 col, u8 index, u8 dir)
{
    u8 r, c;
    u16 bit = 0x0008;
    u16 val = shape_array[index][dir].val;
    for (r = 0; r < 4; r++)
    {
        bit = 0x0008;
        for (c = 0; c < 4; c++)
        {
            if ((val & bit) != 0)
            {
                map[row + r][col + c] = TETRIS_BACK_COLOR;
            }
            bit >>= 1;
        }
        val >>= 4;
    }
}
//记录<row,col>位置方块在地图上的标记
void Set_Block_Flags(u16 row, u16 col, u8 index, u8 dir)
{
    u8 r, c;
    u16 bit = 0x0008;
    u16 val = shape_array[index][dir].val;
    for (r = 0; r < 4; r++)
    {
        bit = 0x0008;
        for (c = 0; c < 4; c++)
        {
            if ((val & bit) != 0)
            {
                map[row + r][col + c] = base_shape_color[index];
            }
            bit >>= 1;
        }
        val >>= 4;
    }
}
//检验<row,col>位置的方块是否有冲突
//有冲突返回 1 否则返回 0
u8 Is_Conflict(u16 row, u16 col, u8 index, u8 dir)
{
    u8 r, c;
    u16 bit = 0x0008;
    u16 val = shape_array[index][dir].val;
    for (r = 0; r < 4; r++)
    {
        bit = 0x0008;
        for (c = 0; c < 4; c++)
        {

            if ((val & bit) != 0 && map[row + r][col + c] != TETRIS_BACK_COLOR)//边界墙或者已经有方块
            {
//              if((row+r)>=(MAP_ROW_SIZE-1))
//              {
//                  return 1;//碰到底部边界墙
//              }
//              if((col+c)<1 || (col+c)>=(MAP_COL_SIZE-1))
//              {
//                  return 1;//碰到左右边界墙
//              }
//              if(map[row + r][col + c] != TETRIS_BACK_COLOR)
//              {
//                   return 1;//已经有
//              }
                return 1;//已经有
            }
            bit >>= 1;
        }
        val >>= 4;
    }
    return 0;
}
void Turn_90(void)
{
    u8 nextdir = (cur_block.dir + 1) % 4;
    Clear_Block_Flags(cur_block.row, cur_block.col, cur_block.index, cur_block.dir); //去除原来的标记
    if (!Is_Conflict(cur_block.row, cur_block.col, cur_block.index, nextdir))
    {
        cur_block.dir = nextdir;
    }
    Set_Block_Flags(cur_block.row, cur_block.col, cur_block.index, cur_block.dir); //增加新的标记
}
void Move_Rigth(void)
{
    Clear_Block_Flags(cur_block.row, cur_block.col, cur_block.index, cur_block.dir); //去除原来的标记
    if (!Is_Conflict(cur_block.row, cur_block.col + 1, cur_block.index, cur_block.dir))
    {
        cur_block.col++;
    }
    Set_Block_Flags(cur_block.row, cur_block.col, cur_block.index, cur_block.dir); //增加新的标记
}
void Move_Left(void)
{
    Clear_Block_Flags(cur_block.row, cur_block.col, cur_block.index, cur_block.dir); //去除原来的标记
    if (!Is_Conflict(cur_block.row, cur_block.col - 1, cur_block.index, cur_block.dir))
    {
        cur_block.col--;
    }
    Set_Block_Flags(cur_block.row, cur_block.col, cur_block.index, cur_block.dir); //增加新的标记
}
void Move_Down(void)
{
    Clear_Block_Flags(cur_block.row, cur_block.col, cur_block.index, cur_block.dir); //去除原来的标记
    if (!Is_Conflict(cur_block.row + 1, cur_block.col, cur_block.index, cur_block.dir))
    {
        cur_block.row++;
        Set_Block_Flags(cur_block.row, cur_block.col, cur_block.index, cur_block.dir); //增加新的标记
    }
    else//显示出来，检查消行 /是否游戏结束
    {
        Set_Block_Flags(cur_block.row, cur_block.col, cur_block.index, cur_block.dir); //增加新的标记

    }
}
//<row,col>位置画基本小方块
void draw_block(u16 row, u16 col, u16 color)
{
    row *= BASE_BLOCK_SIZE; //
    col *= BASE_BLOCK_SIZE; //

    if (TETRIS_BACK_COLOR != color)
    {
        LCD_Fill(col, row, col + BASE_BLOCK_SIZE, row + BASE_BLOCK_SIZE, color);
        LCD_DrawRectangle(col,  row,  col + BASE_BLOCK_SIZE, row + BASE_BLOCK_SIZE);//不是背景色则画框
    }

}
void Show_Game_Board(void)
{

    u16 row, col;
    LCD_Fill(BASE_BLOCK_SIZE, BASE_BLOCK_SIZE, BASE_BLOCK_SIZE * (MAP_COL_SIZE - 1),
             (MAP_ROW_SIZE - 1)*BASE_BLOCK_SIZE, WHITE);
    for (row = 1; row < MAP_ROW_SIZE-1; row++)
    {
        for (col = 1; col < MAP_COL_SIZE-1; col++)
        {
            draw_block(row, col, map[row][col]);
        }
    }

}
void Tetris_Tmr_Callback(void *ptmr, void *parg)
{
//  LCD_Clear(WHITE);
//  LCD_Fill(0,0,MAP_COL_PXIEL_MAX,MAP_ROW_PIXEL_MAX,WHITE);
//  LCD_DrawRectangle(0,  0,MAP_COL_SIZE*BASE_BLOCK_SIZE, MAP_ROW_SIZE*BASE_BLOCK_SIZE);
//  u16 row,col;
//  for(row=0;row<MAP_ROW_SIZE;row++)
//  {
//      for(col=0;col<MAP_COL_SIZE;col++)
//      {
////            if(TETRIS_BACK_COLOR!=map[row][col])
////            {
//              draw_block(col,row,map[row][col]);
////            }
//      }
//  }
//
//  draw_shape(tetris_para.cursor.row,tetris_para.cursor.col,
////             tetris_para.next_shape.index,tetris_para.next_shape.dir);
//    static INT32U last_ticks = 0;
//    INT32U ticks = OSTimeGet();

//    printf("%s --> %d\r\n", __FUNCTION__, (ticks - last_ticks));
//    last_ticks = ticks;
}
void Random_Number_Init(void)
{
	RNG_DeInit();
	RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_RNG, ENABLE);
	RNG_Cmd(ENABLE);
}
u32 Get_Random_Number(void)
{
	while(RNG_GetFlagStatus(RNG_FLAG_DRDY)==RESET);//等待随机数就绪
	return RNG_GetRandomNumber();
}
void Get_Next_Shape(block_t *next)
{
	u32 val1,val2;
	val1=Get_Random_Number();
	val2=Get_Random_Number();
	printf("RandomNumber 1:%d\r\n",val1);
	printf("RandomNumber 2:%d\r\n",val2);
    next->row = 1;
    next->col = MAP_COL_SIZE / 2;
    next->dir = val1%4;//rand()%4;
    next->index = val2%7;//rand()%7;
}
void Init_Map(void)
{
    u16 row, col;
    col = 0;
    for (row = 0; row < MAP_ROW_SIZE; row++)
    {
        map[row][col] = TETRIS_WALL_COLOR;
    }
    col = MAP_COL_SIZE - 1;
    for (row = 0; row < MAP_ROW_SIZE; row++)
    {
        map[row][col] = TETRIS_WALL_COLOR;
    }
    row = 0;
    for (col = 0; col < MAP_COL_SIZE; col++)
    {
        map[row][col] = TETRIS_WALL_COLOR;
    }
    row = MAP_ROW_SIZE - 1;
    for (col = 0; col < MAP_COL_SIZE; col++)
    {
        map[row][col] = TETRIS_WALL_COLOR;
    }
    for (row = 1; row < MAP_ROW_SIZE - 1; row++)
    {
        for (col = 1; col < MAP_COL_SIZE - 1; col++)
        {
            map[row][col] = TETRIS_BACK_COLOR;
        }
    }
	//--TODO
	Set_Block_Flags(12,10,5,2);
	
	Set_Block_Flags(18,9,4,1);
}
void Draw_Wall(void)
{
    u16 row, col;
    col = 0;
    for (row = 0; row < MAP_ROW_SIZE; row++)
    {

        draw_block(row,  col, TETRIS_WALL_COLOR);
    }
    col = MAP_COL_SIZE - 1;
    for (row = 0; row < MAP_ROW_SIZE; row++)
    {
        draw_block(row,  col, TETRIS_WALL_COLOR);
    }
    row = 0;
    for (col = 0; col < MAP_COL_SIZE; col++)
    {
        draw_block(row,  col, TETRIS_WALL_COLOR);
    }
    row = MAP_ROW_SIZE - 1;
    for (col = 0; col < MAP_COL_SIZE; col++)
    {
        draw_block(row,  col, TETRIS_WALL_COLOR);
    }
}
void Tetris_Init(void)
{
	//
	Random_Number_Init();
//    u16 row, col;
    //绘制地图边框
    Draw_Wall();
//    LCD_DrawRectangle(0,  0, MAP_COL_SIZE * BASE_BLOCK_SIZE, MAP_ROW_SIZE * BASE_BLOCK_SIZE);
//    printf("col:%d , row:%d\n", MAP_COL_SIZE * BASE_BLOCK_SIZE, MAP_ROW_SIZE * BASE_BLOCK_SIZE);
//  LCD_DrawRectangle(0,  0,479, 600);
//    show_all_shape();
    POINT_COLOR = RED;
    LCD_ShowString(10, 650, 50, 16, 16, (u8 *)"SCORE:"); //显示一个字符串,12/16字体
    LCD_ShowxNum(70, 650, 3456, 4, 16, 0);              //显示 数字
    LCD_ShowString(10, 700, 80, 16, 16, (u8 *)"NEXT:"); //显示一个字符串,12/16字体
    POINT_COLOR = BLACK;
    Init_Map();
    Get_Next_Shape(&next_block);
    draw_shape(10, 700 / BASE_BLOCK_SIZE, next_block.index, next_block.dir);
    memcpy(&cur_block, &next_block, sizeof(block_t));
    Set_Block_Flags(cur_block.row, cur_block.col, cur_block.index, cur_block.dir);
    Show_Game_Board();

}

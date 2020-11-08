#include "tetris.h"
#include  "lcd.h"
#include  "delay.h"

//基本小方块大小
#define BASE_BLOCK_SIZE     10//20*20

#define ROW_PIXEL_MAX       600//<=SSD_HOR_RESOLUTION
#define COL_PXIEL_MAX       SSD_VER_RESOLUTION//480

//地图
u8 map[30][20];//
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
            .ele.row0 = 0x8, //1000
            .ele.row1 = 0x8, //1000
            .ele.row2 = 0x8, //1000
            .ele.row3 = 0x8, //1000
        },
        {
            .ele.row0 = 0x0, //0000
            .ele.row1 = 0xf, //1111
            .ele.row2 = 0x0, //0000
            .ele.row3 = 0x0, //0000
        },
        {
            .ele.row0 = 0x8, //1000
            .ele.row1 = 0x8, //1000
            .ele.row2 = 0x8, //1000
            .ele.row3 = 0x8, //1000
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
            .ele.row1 = 0xe, //1110
            .ele.row2 = 0x4, //0100
            .ele.row3 = 0x0, //0000
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
            .ele.row0 = 0x6, //0100
            .ele.row1 = 0x4, //0100
            .ele.row2 = 0x4, //1100
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


void draw_shape1(void)
{
    u8 i = 0, j;
    u16 bit = 0x0008;
    u16 x = 100, y = 100;
    u16 x1 = x, y1 = y;
    u16 val;
    u16 color;
    for (i = 0; i < 7; i++)
    {
        val = shape_array[i][0].val;
        color = base_shape_color[i];
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
            y += BASE_BLOCK_SIZE;
            x1 = x;
            y1 = y;
        }
        y += BASE_BLOCK_SIZE;
        y1 = y;
    }
}
void draw_shape2(u16 x,u16 y,u8 shape_index,u8 dir)
{
    u8  j;
    u16 bit = 0x0008;
    u16 x1 , y1 ;
    u16 val;
    u16 color;
	x*=(BASE_BLOCK_SIZE*4);//每个方块由4行4列基本小方块组成
	y*=(BASE_BLOCK_SIZE*4);//
	x1=x;
	y1=y;
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
		y += BASE_BLOCK_SIZE;
		x1 = x;
		y1 = y;
	}   
}

void draw_shape(void)
{
	u8 dir,index,x,y;
	x=2;
	y=0;
	for(index=0;index<7;index++)
	{
		for(dir=0;dir<4;dir++)
		{
			draw_shape2(x++,y,index,dir);
			x++;
		}
		x=2;
		y+=2;
	}
}


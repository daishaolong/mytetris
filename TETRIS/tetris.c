#include "tetris.h"
#include  "lcd.h"
#include  "delay.h"
#include "usart.h"
#include  <string.h>
#include  <stdlib.h>
#if SYSTEM_SUPPORT_OS
    #include "includes.h"                   //ucos ʹ��
#endif

u16 kill_lines;
u32 run_time;
u32 begin_time;
u8 game_over = 0;
u8 game_level=1;

//��ͼ
u8 map[MAP_ROW_SIZE][MAP_COL_SIZE];//15*12
u8 base_shape_val[7]={SHAPE_0,SHAPE_1,SHAPE_2,SHAPE_3,SHAPE_4,SHAPE_5,SHAPE_6};

//7�ֻ����ṹ��ÿ����4����ת����
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



block_t cur_block;//��ǰ��
block_t next_block;//��һ����
void Random_Number_Init(void)
{
    RNG_DeInit();
    RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_RNG, ENABLE);
    RNG_Cmd(ENABLE);
}
u32 Get_Random_Number(void)
{
    while (RNG_GetFlagStatus(RNG_FLAG_DRDY) == RESET); //�ȴ����������
    return RNG_GetRandomNumber();
}
void Get_Shape(block_t *next)
{
    u32 val1, val2;
    val1 = Get_Random_Number();
    val2 = Get_Random_Number();
    next->row = 1;
    next->col = MAP_COL_SIZE / 2 -1;
    next->dir = val1 % 4; //rand()%4;
    next->index = val2 % 7; //rand()%7;
	//����4�����ڷŷ��飬����ʾ����
	//һ�²����ܱ�֤�ڵ�5��¶������������һ��
	element_t e=shape_array[next->index][next->dir].ele;
	if(e.row3!=0)
	{
		return ;
	}
	next->row++;
	if(e.row2!=0)
	{
		return ;
	}
	next->row++;
	if(e.row1!=0)
	{
		return ;
	}
	next->row++;
	
}
//���<row,col>λ�÷����ڵ�ͼ�ϵı��
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
                map[row + r][col + c] = NONE;
            }
            bit >>= 1;
        }
        val >>= 4;
    }
}
//��¼<row,col>λ�÷����ڵ�ͼ�ϵı��
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
                map[row + r][col + c] = base_shape_val[index];
            }
            bit >>= 1;
        }
        val >>= 4;
    }
}
//����<row,col>λ�õķ����Ƿ��г�ͻ
//�г�ͻ���� 1 ���򷵻� 0
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
            if ((val & bit) != 0 && map[row + r][col + c] != NONE)//�߽�ǽ�����Ѿ��з���
            {
                return 1;//�Ѿ���  --TODO
            }
            bit >>= 1;
        }
        val >>= 4;
    }
    return 0;
}

void Turn_90(void)
{
    if (1 == game_over)
    {
        return ;
    }
    u8 nextdir = (cur_block.dir + 1) % 4;
    Clear_Block_Flags(cur_block.row, cur_block.col, cur_block.index, cur_block.dir); //ȥ��ԭ���ı��
    if (!Is_Conflict(cur_block.row, cur_block.col, cur_block.index, nextdir))
    {
        cur_block.dir = nextdir;
    }
    Set_Block_Flags(cur_block.row, cur_block.col, cur_block.index, cur_block.dir); //�����µı��
}
void Move_Rigth(void)
{
    if (1 == game_over)
    {
        return ;
    }
//  printf("Move_Rigth row:%d col:%d\r\n",cur_block.row,cur_block.col);
    Clear_Block_Flags(cur_block.row, cur_block.col, cur_block.index, cur_block.dir); //ȥ��ԭ���ı��
    if (!Is_Conflict(cur_block.row, cur_block.col + 1, cur_block.index, cur_block.dir))
    {
        cur_block.col++;
    }
    Set_Block_Flags(cur_block.row, cur_block.col, cur_block.index, cur_block.dir); //�����µı��
}
void Move_Left(void)
{
    if (1 == game_over)
    {
        return ;
    }
    Clear_Block_Flags(cur_block.row, cur_block.col, cur_block.index, cur_block.dir); //ȥ��ԭ���ı��
    if (!Is_Conflict(cur_block.row, cur_block.col - 1, cur_block.index, cur_block.dir))
    {
        cur_block.col--;
    }

    Set_Block_Flags(cur_block.row, cur_block.col, cur_block.index, cur_block.dir); //�����µı��

}
u8 Is_Full_Line(u16 row)
{
    u16 i;
    for (i = LEFT_WALL_SIZE; i < MAP_COL_SIZE - RIGHT_WALL_SIZE; i++)
    {
        if (NONE == map[row][i] || WALL==map[row][i])
        {
            return 0;
        }
    }
    return 1;
}
u8 Is_Empty_Line(u16 row)
{
    u16 i;
    for (i = LEFT_WALL_SIZE; i < MAP_COL_SIZE - RIGHT_WALL_SIZE; i++)
    {
        if (NONE != map[row][i])
        {
            return 0;
        }
    }
    return 1;
}
//������һ���Ƿ�Ϊ����
void Kill_Line(u16 row)
{
    u16 i;
    for (i = LEFT_WALL_SIZE; i < MAP_COL_SIZE - RIGHT_WALL_SIZE; i++)
    {
        map[row][i] = NONE; //���
    }
}
void Check_Lines(u16 row)
{
    s16  i;
    s16 downmost_empty_row = -1;
    for (i = SHAPE_ROW_SIZE - 1; i >= 0; i--)
    {
        if (Is_Full_Line(row + i))
        {
			kill_lines++;
            Kill_Line(row + i);
            if (-1 == downmost_empty_row)
            {
                downmost_empty_row = row + i;
            }
        }
    }
    if (-1 == downmost_empty_row)
    {
        return ;
    }
    for (i = downmost_empty_row - 1; i > 0; i--)
    {
        if (!Is_Empty_Line(i))
        {
            memcpy(map[downmost_empty_row], map[i], sizeof(map[i]));
            Kill_Line(i);
            downmost_empty_row--;
        }
    }
}

void Move_Bottom(void)
{
	u8 status=1;
	while(status)
	{
		status=Move_Down();
	}
}
//���������Ƿ�ɹ�
u8 Move_Down(void)
{
	u16 row,col;
    if (1 == game_over)
    {
        return 0;
    }
    Clear_Block_Flags(cur_block.row, cur_block.col, cur_block.index, cur_block.dir); //ȥ��ԭ���ı��
    if (!Is_Conflict(cur_block.row + 1, cur_block.col, cur_block.index, cur_block.dir))
    {
        cur_block.row++;
        Set_Block_Flags(cur_block.row, cur_block.col, cur_block.index, cur_block.dir); //�����µı��
		return 1;
    }
    else//��ʾ������������� /�Ƿ���Ϸ����
    {
        Set_Block_Flags(cur_block.row, cur_block.col, cur_block.index, cur_block.dir); //�����µı��
        Check_Lines(cur_block.row);


        memcpy(&cur_block, &next_block, sizeof(block_t));
        if (!Is_Conflict(cur_block.row, cur_block.col, cur_block.index, cur_block.dir))
        {
            Set_Block_Flags(cur_block.row, cur_block.col, cur_block.index, cur_block.dir);
            Get_Shape(&next_block);
        }
        else
        {
            //��Ϸ����
            printf("Game over!!!!\r\n");
			for(row=0;row<MAP_ROW_SIZE;row++)
			{
				printf("row:%2d [",row);
				for(col=0;col<MAP_COL_SIZE;col++)
				{
					printf("%d ",map[row][col]);
				}
				printf("]\r\n");
			}
            game_over = 1;
        }
		return 0;
    }
}


extern OS_EVENT  * board_sem;
void Tetris_Tmr_Callback(void *ptmr, void *parg)
{

	run_time=(OSTimeGet()-begin_time)*(1000/OS_TICKS_PER_SEC);
	Move_Down();
	OSSemPost (board_sem);
}

void Init_Map(void)
{
    u16 row, col;
    	
    for(col = 0;col<LEFT_WALL_SIZE;col++)
	{
		for (row = 0; row < MAP_ROW_SIZE; row++)
		{
			map[row][col] = WALL;
		}		
	}

    for(col = MAP_COL_SIZE - RIGHT_WALL_SIZE;col<MAP_COL_SIZE;col++)
	{
		for (row = 0; row < MAP_ROW_SIZE; row++)
		{
			map[row][col] = WALL;
		}		
	}

    row = 0;
    for (col = 0; col < MAP_COL_SIZE; col++)
    {
        map[row][col] = WALL;//
    }
    for(row = MAP_ROW_SIZE - DOWN_WALL_SIZE;row<MAP_ROW_SIZE;row++)
	{
		for (col = 0; col < MAP_COL_SIZE; col++)
		{
			map[row][col] = WALL;
		}		
	}
    for (row = 0; row < MAP_ROW_SIZE - DOWN_WALL_SIZE; row++)
    {
        for (col = LEFT_WALL_SIZE; col < MAP_COL_SIZE - RIGHT_WALL_SIZE; col++)
        {
            map[row][col] = NONE;
        }
    }
}
void Tetris_Init(void)
{
	kill_lines=0;
	begin_time=OSTimeGet();
	game_over=0;
    Random_Number_Init();
    Init_Map();
    Get_Shape(&cur_block);
    Get_Shape(&next_block);
    Set_Block_Flags(cur_block.row, cur_block.col, cur_block.index, cur_block.dir);
}

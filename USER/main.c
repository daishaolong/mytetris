#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "includes.h"
#include "key.h"
#include "lcd.h"
#include "tetris.h"



//ALIENTEK 探索者STM32F407开发板 UCOS实验1
//UCOSII 移植
//技术支持：www.openedv.com
//广州市星翼电子科技有限公司

//START 任务
//设置任务优先级
#define START_TASK_PRIO         10  ///开始任务的优先级为最低
//设置任务堆栈大小
#define START_STK_SIZE          128
//任务任务堆栈
OS_STK START_TASK_STK[START_STK_SIZE];
//任务函数
void start_task(void *pdata);

//LED0任务
//设置任务优先级
#define LED0_TASK_PRIO          7
//设置任务堆栈大小
#define LED0_STK_SIZE           128
//任务堆栈
OS_STK LED0_TASK_STK[LED0_STK_SIZE];
//任务函数
void led0_task(void *pdata);

//LED1任务
//设置任务优先级
#define LED1_TASK_PRIO          6
//设置任务堆栈大小
#define LED1_STK_SIZE           128
//任务堆栈
OS_STK LED1_TASK_STK[LED1_STK_SIZE];
//任务函数
void led1_task(void *pdata);

//浮点测试任务
#define KEY_TASK_PRIO           5
//设置任务堆栈大小
#define KEY_STK_SIZE            128
//任务堆栈
//如果任务中使用printf来打印浮点数据的话一点要8字节对齐
__align(8) OS_STK KEY_TASK_STK[KEY_STK_SIZE];
//任务函数
void key_task(void *pdata);

OS_TMR  * tetris_tmr;

int main(void)
{
    delay_init(168);       //延时初始化
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //中断分组配置
    uart_init(115200);    //串口波特率设置
    LED_Init();  //LED初始化
    LCD_Init();  //LCD初始化
    KEY_Init();  //KEY初始化
    Tetris_Init();//俄罗斯方块初始化
    OSInit();  //UCOS初始化
    OSTaskCreate(start_task, (void *)0, (OS_STK *)&START_TASK_STK[START_STK_SIZE - 1], START_TASK_PRIO); //创建开始任务
    OSStart(); //开始任务
}

//开始任务
void start_task(void *pdata)
{
	INT8U           err;
    OS_CPU_SR cpu_sr = 0;
    pdata = pdata;
    OSStatInit();  //开启统计任务

    OS_ENTER_CRITICAL();  //进入临界区(关闭中断)
	
	tetris_tmr= OSTmrCreate (0,10,OS_TMR_OPT_PERIODIC,Tetris_Tmr_Callback,(void            *)0,
							(INT8U *)"Tetris_Tmr", &err);//100MS周期
	OSTmrStart(tetris_tmr,&err);
    OSTaskCreate(led0_task, (void *)0, (OS_STK *)&LED0_TASK_STK[LED0_STK_SIZE - 1], LED0_TASK_PRIO); //创建LED0任务
    OSTaskCreate(led1_task, (void *)0, (OS_STK *)&LED1_TASK_STK[LED1_STK_SIZE - 1], LED1_TASK_PRIO); //创建LED1任务
    OSTaskCreate(key_task, (void *)0, (OS_STK *)&KEY_TASK_STK[KEY_STK_SIZE - 1], KEY_TASK_PRIO); //创建浮点测试任务
    OSTaskSuspend(START_TASK_PRIO);//挂起开始任务
    OS_EXIT_CRITICAL();  //退出临界区(开中断)
}


//LED0任务
void led0_task(void *pdata)
{
    while (1)
    {
        LED0 = 0;
        delay_ms(80);
        LED0 = 1;
        delay_ms(100);
    }
}

//LED1任务
void led1_task(void *pdata)
{
    while (1)
    {
        LED1 = 0;
        delay_ms(300);
        LED1 = 1;
        delay_ms(300);
    }
}

//浮点测试任务
void key_task(void *pdata)
{
    u8 key;
    while (1)
    {
        key = KEY_Scan(1);
        switch (key)
        {
			case KEY0_PRES:
				Move_Rigth();
				break;
			case KEY1_PRES:
				Move_Down();
				break;
			case KEY2_PRES:
				Move_Left();
				break;
			case WKUP_PRES:
				Turn_90();
				break;
			default:
				break;
        }
        if (key != 0)
        {
			Show_Game_Board();
            printf("key :%d\r\n", key);
        }

       OSTimeDlyHMSM(0, 0, 0, 80);
    }
}

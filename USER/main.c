#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "includes.h"
#include "key.h"
#include "lcd.h"
#include "tetris.h"



//ALIENTEK ̽����STM32F407������ UCOSʵ��1
//UCOSII ��ֲ
//����֧�֣�www.openedv.com
//������������ӿƼ����޹�˾

//START ����
//�����������ȼ�
#define START_TASK_PRIO         10  ///��ʼ��������ȼ�Ϊ���
//���������ջ��С
#define START_STK_SIZE          128
//���������ջ
OS_STK START_TASK_STK[START_STK_SIZE];
//������
void start_task(void *pdata);

//LED0����
//�����������ȼ�
#define LED0_TASK_PRIO          7
//���������ջ��С
#define LED0_STK_SIZE           128
//�����ջ
OS_STK LED0_TASK_STK[LED0_STK_SIZE];
//������
void led0_task(void *pdata);

//LED1����
//�����������ȼ�
#define LED1_TASK_PRIO          6
//���������ջ��С
#define LED1_STK_SIZE           128
//�����ջ
OS_STK LED1_TASK_STK[LED1_STK_SIZE];
//������
void led1_task(void *pdata);

//�����������
#define KEY_TASK_PRIO           5
//���������ջ��С
#define KEY_STK_SIZE            128
//�����ջ
//���������ʹ��printf����ӡ�������ݵĻ�һ��Ҫ8�ֽڶ���
__align(8) OS_STK KEY_TASK_STK[KEY_STK_SIZE];
//������
void key_task(void *pdata);

OS_TMR  * tetris_tmr;

int main(void)
{
    delay_init(168);       //��ʱ��ʼ��
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //�жϷ�������
    uart_init(115200);    //���ڲ���������
    LED_Init();  //LED��ʼ��
    LCD_Init();  //LCD��ʼ��
    KEY_Init();  //KEY��ʼ��
    Tetris_Init();//����˹�����ʼ��
    OSInit();  //UCOS��ʼ��
    OSTaskCreate(start_task, (void *)0, (OS_STK *)&START_TASK_STK[START_STK_SIZE - 1], START_TASK_PRIO); //������ʼ����
    OSStart(); //��ʼ����
}

//��ʼ����
void start_task(void *pdata)
{
	INT8U           err;
    OS_CPU_SR cpu_sr = 0;
    pdata = pdata;
    OSStatInit();  //����ͳ������

    OS_ENTER_CRITICAL();  //�����ٽ���(�ر��ж�)
	
	tetris_tmr= OSTmrCreate (0,10,OS_TMR_OPT_PERIODIC,Tetris_Tmr_Callback,(void            *)0,
							(INT8U *)"Tetris_Tmr", &err);//100MS����
	OSTmrStart(tetris_tmr,&err);
    OSTaskCreate(led0_task, (void *)0, (OS_STK *)&LED0_TASK_STK[LED0_STK_SIZE - 1], LED0_TASK_PRIO); //����LED0����
    OSTaskCreate(led1_task, (void *)0, (OS_STK *)&LED1_TASK_STK[LED1_STK_SIZE - 1], LED1_TASK_PRIO); //����LED1����
    OSTaskCreate(key_task, (void *)0, (OS_STK *)&KEY_TASK_STK[KEY_STK_SIZE - 1], KEY_TASK_PRIO); //���������������
    OSTaskSuspend(START_TASK_PRIO);//����ʼ����
    OS_EXIT_CRITICAL();  //�˳��ٽ���(���ж�)
}


//LED0����
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

//LED1����
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

//�����������
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

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
//�������������ӿƼ����޹�˾

//START ����
//�����������ȼ�
#define START_TASK_PRIO			10  ///��ʼ��������ȼ�Ϊ���
//���������ջ��С
#define START_STK_SIZE			128
//���������ջ
OS_STK START_TASK_STK[START_STK_SIZE];
//������
void start_task(void *pdata);

//LED0����
//�����������ȼ�
#define LED0_TASK_PRIO			7
//���������ջ��С
#define LED0_STK_SIZE			128
//�����ջ
OS_STK LED0_TASK_STK[LED0_STK_SIZE];
//������
void led0_task(void *pdata);

//LED1����
//�����������ȼ�
#define LED1_TASK_PRIO			6
//���������ջ��С
#define LED1_STK_SIZE			128
//�����ջ
OS_STK LED1_TASK_STK[LED1_STK_SIZE];
//������
void led1_task(void *pdata);

//�����������
#define FLOAT_TASK_PRIO			5
//���������ջ��С
#define FLOAT_STK_SIZE			128
//�����ջ
//���������ʹ��printf����ӡ�������ݵĻ�һ��Ҫ8�ֽڶ���
__align(8) OS_STK FLOAT_TASK_STK[FLOAT_STK_SIZE]; 
//������
void float_task(void *pdata);


extern void draw_shape(void);
int main(void)
{
	delay_init(168);       //��ʱ��ʼ��
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //�жϷ�������
	uart_init(115200);    //���ڲ���������
	LED_Init();  //LED��ʼ��
	LCD_Init();  //LCD��ʼ��
	KEY_Init();  //KEY��ʼ��
//	LCD_Fill( 0, 0, 450, 750, BLUE);
//	LCD_DrawRectangle( 0,  0,  450,  750);	
//	LCD_DrawRectangle( 0,  0,  450/2,  750/2);
	draw_shape();
	OSInit();  //UCOS��ʼ��
	OSTaskCreate(start_task,(void*)0,(OS_STK*)&START_TASK_STK[START_STK_SIZE-1],START_TASK_PRIO); //������ʼ����
	OSStart(); //��ʼ����
}

//��ʼ����
void start_task(void *pdata)
{
	OS_CPU_SR cpu_sr=0;
	pdata=pdata;
	OSStatInit();  //����ͳ������
	
	OS_ENTER_CRITICAL();  //�����ٽ���(�ر��ж�)
	OSTaskCreate(led0_task,(void*)0,(OS_STK*)&LED0_TASK_STK[LED0_STK_SIZE-1],LED0_TASK_PRIO);//����LED0����
	OSTaskCreate(led1_task,(void*)0,(OS_STK*)&LED1_TASK_STK[LED1_STK_SIZE-1],LED1_TASK_PRIO);//����LED1����
	OSTaskCreate(float_task,(void*)0,(OS_STK*)&FLOAT_TASK_STK[FLOAT_STK_SIZE-1],FLOAT_TASK_PRIO);//���������������
	OSTaskSuspend(START_TASK_PRIO);//����ʼ����
	OS_EXIT_CRITICAL();  //�˳��ٽ���(���ж�)
}
 

//LED0����
void led0_task(void *pdata)
{
	while(1)
	{
		LED0=0; 
		delay_ms(80);
		LED0=1;
		delay_ms(100);
	}
}

//LED1����
void led1_task(void *pdata)
{
	while(1)
	{
		LED1=0;
		delay_ms(300);
		LED1=1;
		delay_ms(300);
	}
}

//�����������
void float_task(void *pdata)
{
	OS_CPU_SR cpu_sr=0;
	static float float_num=0.01;
//	u8 x=0b10001000;
	while(1)
	{
		float_num+=0.01f;
		OS_ENTER_CRITICAL();	//�����ٽ���(�ر��ж�)
		printf("float_num��ֵΪ: %f\r\n",float_num); //���ڴ�ӡ���
		
//		printf("x:%d\r\n",x);
		OS_EXIT_CRITICAL();		//�˳��ٽ���(���ж�)
		delay_ms(500);
	}
}
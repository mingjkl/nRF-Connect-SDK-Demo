//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//�о�԰����
//���̵�ַ��http://shop73023976.taobao.com
//
//  �� �� ��   : main.c
//  �� �� ��   : v2.0
//  ��    ��   : HuangKai
//  ��������   : 2018-10-31
//  ����޸�   : 
//  ��������   :��ʾ����(STM32F103ϵ��)
//              ˵��: 
//              ----------------------------------------------------------------
//              GND   ��Դ��
//              VCC   3.3v��Դ
//              SCL   PA5��SCLK��
//              SDA   PA7��MOSI��
//              RES   PD2
//              DC    PB5
//              CS    PA4
//              BLK   PB6
//              ----------------------------------------------------------------
// �޸���ʷ   :
// ��    ��   : 
// ��    ��   : HuangKai
// �޸�����   : �����ļ�
//��Ȩ���У�����ؾ���
//Copyright(C) �о�԰����2018-10-31
//All rights reserved
//******************************************************************************/
#include "delay.h"
#include "sys.h"
#include "led.h"
#include "lcd_init.h"
#include "lcd.h"
#include "pic.h"
int main(void)
{
	u8 i,j;
	float t=0;
	delay_init();
	LED_Init();//LED��ʼ��
	LCD_Init();//LCD��ʼ��
	LCD_Fill(0,0,LCD_W,LCD_H,WHITE);
	LED=0;
	while(1)
	{
		LCD_ShowChinese(0,0,"�о�԰����",RED,WHITE,32,0);
		LCD_ShowString(0,40,"LCD_W:",RED,WHITE,16,0);
		LCD_ShowIntNum(48,40,LCD_W,3,RED,WHITE,16);
		LCD_ShowString(80,40,"LCD_H:",RED,WHITE,16,0);
		LCD_ShowIntNum(128,40,LCD_H,3,RED,WHITE,16);
		LCD_ShowString(80,40,"LCD_H:",RED,WHITE,16,0);
		LCD_ShowString(0,70,"Increaseing Nun:",RED,WHITE,16,0);
		LCD_ShowFloatNum1(128,70,t,4,RED,WHITE,16);
		t+=0.11;
		for(j=0;j<3;j++)
		{
			for(i=0;i<6;i++)
			{
				LCD_ShowPicture(40*i,120+j*40,40,40,gImage_1);
			}
		}
	}
}


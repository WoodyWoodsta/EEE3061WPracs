/*
 * LED.c
 *
 *  Created on: 17 Feb 2015
 *      Author: Student
 */


#include "LED.h"


void init_leds(void)
{

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB,ENABLE);

	GPIO_InitTypeDef 	GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;// set the pins low
	GPIO_Init(GPIOB, &GPIO_InitStructure);


	GPIO_SetBits(GPIOB,GPIO_Pin_7);//set the pin high
	GPIO_ResetBits(GPIOB,GPIO_Pin_6);//set the pin low
	GPIO_SetBits(GPIOB,GPIO_Pin_5);
	GPIO_ResetBits(GPIOB,GPIO_Pin_4);
	GPIO_SetBits(GPIOB,GPIO_Pin_3);
	GPIO_ResetBits(GPIOB,GPIO_Pin_2);
	GPIO_SetBits(GPIOB,GPIO_Pin_1);
	GPIO_ResetBits(GPIOB,GPIO_Pin_0);

}


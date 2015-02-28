//
// EEE3061W Prac 1
// Author: Callen Fisher and Sean Wood
//

#define STM32F051

// ----------------------------------------------------------------------------

#include <stdio.h>
#include "diag/Trace.h"
#include "stm32f0xx.h"
#include "stm32f0xx_gpio.h"
#include <stm32f0xx_tim.h>
#include "LED.h"
#include "lcd_stm32f0.h"

void main(void);
void init_PWM(void);
void setGreen(uint8_t percentage);
void setRed(uint8_t percentage);
void init_adc_POTs(void);
void DMA_ADC_init(void);

uint8_t val0;
uint8_t val1;
uint8_t RegularConvData_Tab[2];

void main(void) {
  volatile uint32_t delay_counter = 0;
  uint32_t i = 0;
  init_PWM();
  init_adc_POTs();
  init_leds();

  // == TASK 1
  lcd_init();
  lcd_string("Hello World!!");

  for (;;) {

    // == TASK 2
    uint8_t pot0Value = RegularConvData_Tab[0] / 2.56;
    uint8_t pot1Value = RegularConvData_Tab[1] / 2.56;
    static uint8_t lastPot0Value;
    static uint8_t lastPot1Value;

    if (pot1Value != lastPot1Value) {
      lcd_command(LCD_GOTO_LINE_2); // Make sure we are on line two
      lcd_string("   "); // Clear leftovers
      lcd_command(LCD_GOTO_LINE_2); // Make sure we are on line two
      LCD_display_uint8(pot1Value);

      lastPot1Value = pot1Value;
    } else {
      for (delay_counter = 0; delay_counter < 655350; delay_counter++)
        ;
      GPIOB->ODR = 0xAA;

      for (delay_counter = 0; delay_counter < 655350; delay_counter++)
        ;
      GPIOB->ODR = 0x55;

    }

    // == TASK 3 and 4
    if (pot0Value != lastPot0Value) {
      setGreen(pot0Value);
      setRed(100 - pot0Value);

      lastPot0Value = pot0Value;
    }

  }
}

void init_adc_POTs(void) {
  DMA_ADC_init();

  // Enable the clocks
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);

  GPIO_InitTypeDef GPIO_InitStructure;

  // Enable the two pins
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN; // Set to analogue mode
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  RCC_ADCCLKConfig(RCC_ADCCLK_HSI14);
  RCC_HSI14Cmd(ENABLE);

  ADC_DeInit(ADC1);

  ADC_InitTypeDef ADC1struct;
  ADC_StructInit(&ADC1struct);

  ADC_StructInit(&ADC1struct);
  ADC1struct.ADC_Resolution = ADC_Resolution_8b; // Set ADC to 8-bit mode
  ADC1struct.ADC_ContinuousConvMode = ENABLE; // Put in continous conversion mode
  ADC1struct.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
  ADC1struct.ADC_DataAlign = ADC_DataAlign_Right;
  ADC1struct.ADC_ScanDirection = ADC_ScanDirection_Upward;
  ADC_Init(ADC1, &ADC1struct);

  // Set up the ADC channels
  ADC_ChannelConfig(ADC1, ADC_Channel_6, ADC_SampleTime_55_5Cycles);
  ADC_ChannelConfig(ADC1, ADC_Channel_5, ADC_SampleTime_55_5Cycles);

  // Link it to the DMA
  ADC_DMARequestModeConfig(ADC1, ADC_DMAMode_Circular);

  ADC_Cmd(ADC1, ENABLE);
  ADC_DMACmd(ADC1, ENABLE);
  while (!ADC_GetFlagStatus(ADC1, ADC_FLAG_ADEN))
    ;
  ADC_StartOfConversion(ADC1);
}

void DMA_ADC_init(void) {
  DMA_InitTypeDef DMA_InitStructure;
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

  DMA_DeInit(DMA1_Channel1);
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) (&(ADC1->DR)); // The address of the ADC
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t) RegularConvData_Tab; // Where the data is stored
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
  DMA_InitStructure.DMA_BufferSize = 2; // Linked to two ADC channels
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular; // Circular because the ADC is in continous mode
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
  DMA_Init(DMA1_Channel1, &DMA_InitStructure);

  DMA_Cmd(DMA1_Channel1, ENABLE);
}

void init_PWM(void) {
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

  GPIO_InitTypeDef GPIO_InitStructure;

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11; // Set up the two pins (PWM for the RG LED)
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; // Set the pins to alternate funtion
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_2); // Map the pins to the relevant alternate function mode
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_2);

  TIM_TimeBaseInitTypeDef TIM_TimeBaseStruct;

  TIM_TimeBaseStruct.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_TimeBaseStruct.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseStruct.TIM_Period = 8000; // Set the frequency
  TIM_TimeBaseStruct.TIM_Prescaler = 0;
  TIM_TimeBaseStruct.TIM_RepetitionCounter = 0x0000;

  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStruct);

  TIM_OCInitTypeDef OC;

  OC.TIM_OCMode = TIM_OCMode_PWM1;
  OC.TIM_OutputState = TIM_OutputState_Enable;
  OC.TIM_Pulse = 4000; // Set the duty cycle
  OC.TIM_OCPolarity = TIM_OCPolarity_Low;

  TIM_OC3Init(TIM2, &OC);
  TIM_OC4PreloadConfig(TIM2, TIM_OCPreload_Enable);
  TIM_OC4Init(TIM2, &OC);
  TIM_OC4PreloadConfig(TIM2, TIM_OCPreload_Enable);

  TIM_ARRPreloadConfig(TIM2, DISABLE);
  //TIM_CtrlPWMOutputs(TIM2, ENABLE);
  TIM_Cmd(TIM2, ENABLE);
  TIM_SetCompare3(TIM2, 8000); // Red        	//NOTE: period is 8000 so that is the max value that can go here
  TIM_SetCompare4(TIM2, 8000); // Green		//NOTE: inversed!!! 8000 is 0% and 0 is 100% duty cycle
}

void setGreen(uint8_t percentage) {
  TIM_SetCompare4(TIM2, (uint16_t) (8000 - percentage * 80)); // Green
}

void setRed(uint8_t percentage) {
  TIM_SetCompare3(TIM2, (uint16_t) (8000 - percentage * 80)); // Red
}

// ----------------------------------------------------------------------------

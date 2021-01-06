#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"

void init_GPIO(void);

void init_GPIO(void){
	GPIO_InitTypeDef  GPIO_InitStructure;

  /* Initialize LED which connected to PC12 */
  // Enable PORTC Clock
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
  /* Configure the GPIO_LED pin */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
}

int main(void){
  init_GPIO();
	
	volatile int i;
    while (1) {
			for (i=0; i < 1000000; i++);
			GPIO_ResetBits(GPIOC, GPIO_Pin_12);

    	for (i=0; i < 1000000; i++);
    	GPIO_SetBits(GPIOC, GPIO_Pin_12);
	}
    	
}

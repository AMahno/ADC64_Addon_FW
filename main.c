#include "main.h"
#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "i2c_slave.h"

void SetSysClockTo72(void){
	ErrorStatus HSEStartUpStatus;
    /* SYSCLK, HCLK, PCLK2 and PCLK1 configuration -----------------------------*/
    /* RCC system reset(for debug purpose) */
    RCC_DeInit();

    /* Enable HSE */
    RCC_HSEConfig( RCC_HSE_ON);

    /* Wait till HSE is ready */
    HSEStartUpStatus = RCC_WaitForHSEStartUp();
		if(1)
    //if (HSEStartUpStatus == SUCCESS)
    {
        /* Enable Prefetch Buffer */
    	//FLASH_PrefetchBufferCmd( FLASH_PrefetchBuffer_Enable);

        /* Flash 2 wait state */
        //FLASH_SetLatency( FLASH_Latency_2);

        /* HCLK = SYSCLK */
        RCC_HCLKConfig( RCC_SYSCLK_Div1);

        /* PCLK2 = HCLK */
        RCC_PCLK2Config( RCC_HCLK_Div1);

        /* PCLK1 = HCLK/2 */
        RCC_PCLK1Config( RCC_HCLK_Div1);

        /* PLLCLK = 8MHz * 9 = 72 MHz */
        //RCC_PLLConfig(0x00010000, RCC_PLLMul_9);
        //RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);
				RCC_PLLConfig(RCC_PLLSource_HSI_Div2, RCC_PLLMul_9);

        /* Enable PLL */
        RCC_PLLCmd( ENABLE);

        /* Wait till PLL is ready */
        while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
        {
        }

        /* Select PLL as system clock source */
        RCC_SYSCLKConfig( RCC_SYSCLKSource_PLLCLK);

        /* Wait till PLL is used as system clock source */
        while (RCC_GetSYSCLKSource() != 0x08)
        {
        }
    }
    else
    { /* If HSE fails to start-up, the application will have wrong clock configuration.
     User can add here some code to deal with this error */
				GPIO_SetBits(GPIOC, GPIO_Pin_13);
        /* Go to infinite loop */
        while (1)
        {
        }
    }
}

void init_GPIO(void){
	GPIO_InitTypeDef  GPIO_InitStructure;

  /* Initialize LED which connected to PC12 */
  // Enable PORTC Clock
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
  /* Configure the GPIO_LED pin */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
}

int main(void){
  init_GPIO();
	SetSysClockTo72();
	I2C1_Slave_init();
		
	volatile int i;
    while (1) {
			for (i=0; i < 1000000; i++);
			GPIO_ResetBits(GPIOC, GPIO_Pin_13);

    	for (i=0; i < 1000000; i++);
    	GPIO_SetBits(GPIOC, GPIO_Pin_13);
	}
    	
}

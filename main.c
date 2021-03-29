#include "main.h"
#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_spi.h"
#include "i2c_slave.h"
/*
#define SPIx_RCC      RCC_APB2Periph_SPI1
#define SPIx          SPI1
#define SPI_GPIO_RCC  RCC_APB2Periph_GPIOA
#define SPI_GPIO      GPIOA
#define SPI_GPIO_SS   GPIOA
#define SPI_PIN_MOSI  GPIO_Pin_7
#define SPI_PIN_MISO  GPIO_Pin_6
#define SPI_PIN_SCK   GPIO_Pin_5
#define SPI_PIN_SS    GPIO_Pin_4
 */
 
#define SPIx_RCC      RCC_APB2Periph_SPI1
#define SPIx          SPI1
#define SPI_GPIO_RCC  RCC_APB2Periph_GPIOA
#define SPI_GPIO_RCC_SS  RCC_APB2Periph_GPIOA
#define SPI_GPIO      GPIOA
#define SPI_GPIO_SS   GPIOA
#define SPI_PIN_MOSI  GPIO_Pin_7
#define SPI_PIN_MISO  GPIO_Pin_6
#define SPI_PIN_SCK   GPIO_Pin_5
#define SPI_PIN_SS    GPIO_Pin_4
 
 
void SPIx_Init(void);
uint16_t SPIx_Transfer(uint8_t data);
void SPIx_EnableSlave(void);
void SPIx_DisableSlave(void);

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
	
	/* Initialize enable pins for DACs*/
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
}


void SPIx_Init()
{
    // Initialization struct
    SPI_InitTypeDef SPI_InitStruct;
    GPIO_InitTypeDef GPIO_InitStruct;
 
    // Step 1: Initialize SPI
    RCC_APB2PeriphClockCmd(SPIx_RCC, ENABLE);
    SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_128;
    //SPI_InitStruct.SPI_CPHA = SPI_CPHA_1Edge;
		SPI_InitStruct.SPI_CPHA = SPI_CPHA_2Edge;
    SPI_InitStruct.SPI_CPOL = SPI_CPOL_Low;
    SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStruct.SPI_Mode = SPI_Mode_Master;
    SPI_InitStruct.SPI_NSS = SPI_NSS_Soft | SPI_NSSInternalSoft_Set;
    SPI_Init(SPIx, &SPI_InitStruct); 
    SPI_Cmd(SPIx, ENABLE);
 
    // Step 2: Initialize GPIO
    RCC_APB2PeriphClockCmd(SPI_GPIO_RCC, ENABLE);
		RCC_APB2PeriphClockCmd(SPI_GPIO_RCC_SS, ENABLE);
		//GPIO_PinRemapConfig(GPIO_Remap_SPI1,ENABLE); 
		//GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);
    // GPIO pins for MOSI, MISO, and SCK
    GPIO_InitStruct.GPIO_Pin = SPI_PIN_MOSI | SPI_PIN_MISO | SPI_PIN_SCK;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(SPI_GPIO, &GPIO_InitStruct);
    // GPIO pin for SS
    GPIO_InitStruct.GPIO_Pin = SPI_PIN_SS;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    //GPIO_Init(SPI_GPIO, &GPIO_InitStruct);
		GPIO_Init(SPI_GPIO_SS, &GPIO_InitStruct);
 
    // Disable SPI slave device
    SPIx_DisableSlave();
}
 
uint16_t SPIx_Transfer(uint8_t data)
{
    // Write data to be transmitted to the SPI data register
    SPIx->DR = data;
    // Wait until transmit complete
    while (!(SPIx->SR & (SPI_I2S_FLAG_TXE)));
    // Wait until receive complete
    while (!(SPIx->SR & (SPI_I2S_FLAG_RXNE)));
    // Wait until SPI is not busy anymore
    while (SPIx->SR & (SPI_I2S_FLAG_BSY));
    // Return received data from SPI data register
    return SPIx->DR;
}
 
void SPIx_EnableSlave()
{
    // Set slave SS pin low
    SPI_GPIO_SS->BRR = SPI_PIN_SS;
}
 
void SPIx_DisableSlave()
{
    // Set slave SS pin high
    SPI_GPIO_SS->BSRR = SPI_PIN_SS;
}

int main(void){
  init_GPIO();
	SetSysClockTo72();
	I2C1_Slave_init();
	SPIx_Init();
	
	GPIO_ResetBits(GPIOB, GPIO_Pin_12);

	
	volatile int i;
    while (1) {
			for (i=0; i < 1000000; i++);
			GPIO_ResetBits(GPIOC, GPIO_Pin_13);
				SPIx_EnableSlave();
				SPIx_Transfer(0x10);
				SPIx_Transfer(0xFF);
				SPIx_Transfer(0xFF);
				for (i=0; i < 10; i++);
				SPIx_DisableSlave();
    	for (i=0; i < 1000000; i++);
    	GPIO_SetBits(GPIOC, GPIO_Pin_13);
	}
    	
}

#include "Libraries/stm32f0xx.h"
#include "Libraries/stm32f0xx_usart.h"
#include "Libraries/stm32f0xx_rcc.h"
#include "Libraries/stm32f0xx_misc.h"
#include "Libraries/stm32f0xx_adc.h"
#include "core_cm3.h"
#include "core_cmFunc.h"
#include "core_cmInstr.h"
#include "cmsis_gcc.h"

void USART1_Init(void)
{
    /* USART configuration structure for USART1 */
    USART_InitTypeDef usart1_init_struct;
    /* Bit configuration structure for GPIOA PIN9 and PIN10 */
    GPIO_InitTypeDef gpioa_init_struct;

    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_AHBPeriph_GPIOA | RCC_APB2Periph_USART1, ENABLE);

    
    usart1_init_struct.USART_BaudRate = 38000;
    usart1_init_struct.USART_WordLength = USART_WordLength_8b;  
    usart1_init_struct.USART_StopBits = USART_StopBits_1;
    usart1_init_struct.USART_Parity = USART_Parity_Odd;
    usart1_init_struct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    usart1_init_struct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_Cmd(USART1, ENABLE);

    USART_Init(USART1, &usart1_init_struct);
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_Init(&NVIC_InitStructure);
    NVIC_EnableIRQ(USART1_IRQn);
    
    ADC_GetConversionValue

}

void USART1_IRQHandler(void)
{   
    
    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {
        if((char)USART_ReceiveData(USART1) != 0)
        {
            
            while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)
            {
                USART_SendData(USART1, "Hallo");
            }
        }
    }
}

int main(void)
{
    USART1_Init();

    for(;;)
    {

    }
}
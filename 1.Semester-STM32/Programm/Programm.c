#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h> 
#include <fcntl.h>
#include <stdint.h>

#include "header.h"

char USART_READ;
uint32_t ADC_Value;

//Prototypes
char USART_write_data[32];
int send=0;
//ADC Interrupt Handler
void ADC1_IRQHandler(void)   
{
    uint32_t* adc_isr = ADC_ISR;
    uint32_t* adc_cr = ADC_CR;

    uint32_t adc_isr_eoc = ADC_ISR_EOC;
    uint32_t adc_isr_adrdy = ADC_ISR_ADRDY;
    uint32_t* adc_dr = ADC_DR;
    uint32_t adc_dr_data = ADC_DR_DATA;
    uint32_t* gpioa_odr = GPIOA_ODR;
    uint32_t* usart1_tdr =  USART1_TDR;
    
    uint32_t adc_cr_adstart = ADC_CR_ADSTART;
    uint16_t ADC_Value;
    
    uint32_t reg_content;

    //if ADC Ready start convertion
    if((*adc_isr & adc_isr_adrdy) == adc_isr_adrdy)
    {
        reg_content = *adc_cr;
        reg_content |= adc_cr_adstart;
        *adc_cr = reg_content;
    }

    //if convertion complete send ADC value
    if((*adc_isr & adc_isr_eoc) == adc_isr_eoc)
    {
        send=0;
        ADC_Value = (*adc_dr & adc_dr_data);
        
        
        sprintf(USART_write_data, "%d", ADC_Value);
    }
}

//Enable Interrupts in NVIC
void NVIC_enable_interrupts(void)
{
    uint32_t* nvic_iser =  NVIC_ISER;
    uint32_t reg_content;
    reg_content = *nvic_iser;
    reg_content |= 0x08001000;
    *nvic_iser = reg_content;
}


//USART Interrupt Handler
void USART1_IRQHandler(void)
{   
    uint32_t* usart1_isr = USART1_ISR;
    uint32_t usart1_isr_rxne = USART1_ISR_RXNE;
    uint32_t usart1_isr_tc = USART1_ISR_TC;
    uint32_t* usart1_tc = USART1_ICR;
    uint32_t* usart1_tc_tcce = USART1_ICR_TCCF;
    uint32_t* usart1_tdr =  USART1_TDR;
    uint32_t* gpioa_odr = GPIOA_ODR;
    uint32_t reg_content;
    
    if ((*usart1_isr & usart1_isr_tc) == usart1_isr_tc)
    {
        if (send == sizeof(USART_write_data)) 
        {
            //set MAX to listen 
            reg_content = *gpioa_odr;
            reg_content |= 0x00000000;
            *gpioa_odr = reg_content;          
            send=0;
            *usart1_tc |= *usart1_tc_tcce; // Clear transfer complete flag *      
        }
        else
        {
            // clear transfer complete flag and fill TDR with a new char 
            *usart1_tdr = USART_write_data[send++];
        }
    }
    
    //when something comes at usart, begin to write the ADC Data 
    if ((*usart1_isr & usart1_isr_rxne) == usart1_isr_rxne)
    {
        USART_READ = *((char *)USART1_RDR);
        //Set PA7 and PA6 high for max to send data
        reg_content = *gpioa_odr;
        reg_content |= 0x000000C0;
        *gpioa_odr = reg_content;

        *usart1_tdr  = USART_write_data[0];
    }
}

//Initialize ADC
int init_ADC()
{   
    uint32_t reg_content;
    uint32_t* adc_chselr = ADC_CHSELR;
    uint32_t* adc_isr = ADC_ISR;       
    uint32_t* adc_cr = ADC_CR;
    uint32_t* adc_cfgr1 = ADC_CFGR1;
    uint32_t adc_cr_adcal = ADC_CR_ADCAL;
    uint32_t* adc_ier = ADC_IER;
    uint32_t* adc_cfgr2 = ADC_CFGR2;
    uint32_t adc_isr_adrdy = ADC_ISR_ADRDY;
    
    //set ADC clock to PCLK so thats is synchronous with sysclock
    reg_content = *adc_cfgr2;
    reg_content |= 0x40000000;
    *adc_cfgr2 = reg_content;

    //before starting callibrate ADC
    reg_content = *adc_cr;
    reg_content |= adc_cr_adcal;
    *adc_cr = reg_content;
    
    //if calibration is complete enable ADC
    //enable Interrupts
    //set correct channel
    //when ADC is ready, a ad ready interrupt occurs and the interrupt handler
    //will then start the ADC convertion
    while((*adc_cr & adc_cr_adcal) == adc_cr_adcal); // wait till callibration is complete

   
    //enable ADC and ensure ADSTART=0 for further configuration and enable ADC
    reg_content = *adc_cr;
    reg_content |= 0x00000001;
    *adc_cr = reg_content;
    
    //set ADC to continues convertion so, that convertion doesn't to start again in Interrupt
    reg_content = *adc_cr;
    reg_content |= 0x0000200;
    *adc_cr = reg_content;

    //enable Interrupts of ADC
    reg_content = *adc_ier;
    reg_content |= 0x00000005;
    *adc_ier = reg_content;

    //Set ADC Channel to channel 0 because PA0 is ADC_IN0
    reg_content = *adc_chselr;
    reg_content |= 0x00000001;
    *adc_chselr = reg_content;
}

//Initialize needed clocks
int init_CLOCK()
{
    uint32_t reg_content;
    uint32_t* rcc_ahbenr = RCC_AHBENR; 
    uint32_t* rcc_apb2enr = RCC_APB2ENR;
    uint32_t* rcc_cfgr = RCC_CFGR;

    //GPIO A port clock enable
    reg_content = *rcc_ahbenr;
    reg_content |= 0x00020000;
    *rcc_ahbenr = reg_content;
    
    //USART1 clock enable
    reg_content = *rcc_apb2enr;
    reg_content |= 0x00004000;
    *rcc_apb2enr = reg_content;

    //set AHB Clock to not divided so same clock as sysclock
    reg_content = *rcc_cfgr;
    reg_content |= 0x00000000;
    *rcc_cfgr = reg_content; 
}

//Initialize GPIOs
int init_GPIO()
{
    //Set GPIO pins PA9 and PA10 alternate function for USART TX and RX, 
    //set PA7 and PA6 output for DE and RE of MAX485 
    //PA0 ADC_IN0 so set it to analog and then in DAC register set PA0 as ADC_IN0 
    uint32_t reg_content;
    uint32_t* gpioa_moder = GPIOA_MODER;
    uint32_t* gpioa_afrh = GPIOA_AFRH;
    uint32_t* gpioa_odr = GPIOA_ODR;

    //set above mentioned Pins
    reg_content = *gpioa_moder;
    reg_content |= 0x00285003;
    *gpioa_moder = reg_content;

    //Set GPIO PA9 as AF=TX and PA10 as AF = RX
    reg_content = *gpioa_afrh;
    reg_content |= 0x00000110;
    *gpioa_afrh = reg_content;

    //Set GPIO PA9 as AF=TX and PA10 as AF = RX
    reg_content = *gpioa_afrh;
    reg_content |= 0x00000110;
    *gpioa_afrh = reg_content;

    
    //Ensure that output gpios are 0 for MAX, to read all the time
    //and only send, when needing to send
    reg_content = *gpioa_odr;
    reg_content |= 0x00000000;
    *gpioa_odr = reg_content;  
}

//Initialize USART
int init_UART()
{   
    uint32_t reg_content;
    uint32_t* rcc_cfr3 = RCC_CFGR3;
    uint32_t* usart1_brr = USART1_BRR;
    uint32_t* usart1_cr1 = USART1_CR1;

    //Use SYSCLK for USART and use Baudrate 38400
    reg_content = *rcc_cfr3;                
    reg_content |= 0x00000001;
    *rcc_cfr3 = reg_content; 
    
    reg_content = *usart1_brr;
    reg_content = 0x00000D00;
    *usart1_brr = reg_content;
    
    //Wordlenght, Parity control enable, Parity selection, interrupt enable, Transmission complete interrupt enable, RXNE interrupt enable
    reg_content = *usart1_cr1;
    reg_content |= 0x000006EC;
    *usart1_cr1 = reg_content;

    reg_content = *usart1_cr1;
    reg_content |= 0x00000001; //enable UART
    *usart1_cr1 = reg_content;
}

int main()
{
    init_CLOCK();
    init_GPIO();
    init_UART();
    init_ADC();
    NVIC_enable_interrupts();
    
    for(;;);
}
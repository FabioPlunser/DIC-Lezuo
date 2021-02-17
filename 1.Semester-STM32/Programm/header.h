#ifndef __header_H
#define __header_H

//boundary addresses at page 37 - 41
/*
General boundaries
*/
#define PERIPHALS       ((uint32_t*)0x40000000)     //Peripherals
#define APBPERIPHALS    PERIPHALS                   //APBPeriphals
#define AHBPERIPHALS    (PERIPHALS + 0x00020000)
#define AHB2PERIPHALS   (PERIPHALS + 0x08000000)

/*
USART1
*/
#define USART1_BASE (APBPERIPHALS+0x00013800) //USART 1 Base Address
//Base + Address offset + Comment + Page in reference Manual
#define USART1_CR1      (USART1_BASE+0x00)   //USART Control Register 1  -- at Page 625
#define USART1_CR2      (USART1_BASE+0x04)   //USART Control Register 2  -- at Page 628
#define USART1_CR3      (USART1_BASE+0x08)   //USART Control Register 3  -- at Page 630
#define USART1_BRR      (USART1_BASE+0x0C)   //USART Baudrate Register   -- at page 632
#define USART1_RQR      (USART1_BASE+0x18)   //USART Request register

#define USART1_ISR      (USART1_BASE+0x1c)   //USART Interrupt and status register   -- at Page 635
#define USART1_ISR_TXE  ((uint32_t)0x00000080)
#define USART1_ISR_TC   ((uint32_t)0x00000040)
#define USART1_ISR_RXNE ((uint32_t)0x00000020)

#define USART1_ICR       (USART1_BASE+0x20)   //USART Interrupt and flag Clear register -- at Page 638
#define USART1_ICR_TCCF  ((uint32_t) 0x00000040) //USART transmission complete clear flag -- at Page 639
#define USART1_RDR       (USART1_BASE+0x24)   //USART Receive Data register   --  at Page 639
#define USART1_TDR       (USART1_BASE+0x28)   //USART Transmit Data register  --  at Page 640

/*
RCC
*/
//From boundary addresses at page 39
#define RCC (AHBPERIPHALS+0x00001000)
//All found from RCC Register Map on Page 125
#define RCC_CR          (RCC+0x00)          //RCC Control register -- at Page 99
#define RCC_CFGR        (RCC+0x04)          //RCC Clock configuration register -- at Page 101 
#define RCC_CIR         (RCC+0x08)          //RCC Clock interrupt register -- at Page 104
#define RCC_APB2RSTR    (RCC+0x0C)          //RCC APB peripheral reset register 2 -- at Page 106
#define RCC_APB1RSTR    (RCC+0x010)         //RCC APB peripheral reset register 1 -- at Page 108
#define RCC_AHBENR      (RCC+0x14)          //RCC AHB peripheral clock enable register -- at Page 111
#define RCC_APB2ENR     (RCC+0x18)          //RCC APB peripheral clock enable register 2-- at Page 112
#define RCC_APB1ENR     (RCC+0x1C)          //RCC APB peripheral clock enable register 1-- at Page 114
#define RCC_BDCR        (RCC+0x20)          //RCC RTC domain control register -- at Page 117
#define RCC_CSR         (RCC+0x24)          //RCC Control/status register -- at Page 119
#define RCC_AHBRSTR     (RCC+0x28)          //RCC AHB peripheral reset register -- at Page 120
#define RCC_CFGR2       (RCC+0x2C)          //RCC Clock configuration register 2 -- at Page 122
#define RCC_CFGR3       (RCC+0x30)          //RCC Clock configuration register 3 -- at Page 123
#define RCC_CR2         (RCC+0x34)          //RCC Clock control register 2 -- at Page 123


/*
GPIOA
*/
#define GPIO_A (AHB2PERIPHALS + 0x00000000)
//Page 142 
#define GPIOA_MODER     (GPIO_A+0x00)       //GPIO port moder register -- at Page 136
#define GPIOA_OTYPER    (GPIO_A+0x04)       //GPIO port output type register -- at Page 136
#define GPIOA_OSPEEDR   (GPIO_A+0x08)       //GPIO port output speed register -- at Page 137
#define GPIOA_PUPDR     (GPIO_A+0x0C)       //GPIO port pull-up/pull-down register -- at Page 137
#define GPIOA_IDR       (GPIO_A+0x10)       //GPIO port input data register -- at Page 138 
#define GPIOA_ODR       (GPIO_A+0x14)       //GPIO port output data register -- at Page 138
#define GPIOA_BSRR      (GPIO_A+0x18)       //GPIO port bis set/reset register -- at Page 138
#define GPIOA_LCKR      (GPIO_A+0x1c)       //GPIO port configuration lock register -- at Page 139
#define GPIOA_AFRL      (GPIO_A+0x20)       //GPIO alternate function low register -- at Page 140
#define GPIOA_AFRH      (GPIO_A+0x024)      //GPIO alternate function high register -- at Page 141
#define GPIOA_BRR       (GPIO_A+0x28)       //GPIO port bit reset register -- at Page 141


/*
ADC
*/
//boundary address at page 40
#define ADC (APBPERIPHALS+0x00012400)
//Page 220
#define ADC_ISR         (ADC+0x00)                   //ADC interrupt and status register -- at Page 207
#define ADC_ISR_AWD     ((uint32_t)0x00000080)        //Analog watchdog flag 
#define ADC_ISR_OVR     ((uint32_t)0x00000010)        //Overrun flag 
#define ADC_ISR_EOSEQ   ((uint32_t)0x00000008)        //End of Sequence flag
#define ADC_ISR_EOC     ((uint32_t)0x00000004)        //End of Conversion 
#define ADC_ISR_EOSMP   ((uint32_t)0x00000002)        //End of sampling flag 
#define ADC_ISR_ADRDY   ((uint32_t)0x00000001)        //ADC Ready 

#define ADC_IER         (ADC+0x04)      //ADC interrupt enable register -- at Page 208

#define ADC_CR          (ADC+0x08)      //ADC control register -- at Page 210
#define ADC_CR_ADCAL    ((uint32_t)0x80000000)  //ADC Calibration
#define ADC_CR_ADSTP    ((uint32_t)0x00000010)  //ADC stop of conversion command
#define ADC_CR_ADSTART  ((uint32_t)0x00000004)  //ADC start of conversion
#define ADC_CR_ADDIS    ((uint32_t)0x00000002)  //ADC disable command
#define ADC_CR_ADEN     ((uint32_t)0x00000001) //ADC enable control


#define ADC_CFGR1       (ADC+0x0C)      //ADC configuration register 1 -- at Page 212
#define ADC_CFGR2       (ADC+0x10)      //ADC configuration register 2 -- at Page 216
#define ADC_SMPR        (ADC+0x14)      //ADC sampling time register -- at Page 216
#define ADC_TR          (ADC+0x20)      //ADC watchdog threshold register -- at Page 217
#define ADC_CHSELR      (ADC+0x28)      //ADC channel selection register -- at Page 218

#define ADC_DR          (ADC+0x40)      //ADC data register -- at Page 218
#define ADC_DR_DATA     ((uint32_t)0x0000FFFF)       //ADC Data Mask
#define ADC_CCR         (ADC+0x308)     //ADC common configuration register -- at Page 219


//Interrupt ant Page 171
#define ADC_IRQn    12         //Address 0x0000 0070
#define USART1_IRQn 27      //Address 0x0000 00AC
//in ARMv6 and stm32f0xx-cortexm0-programming-manual-- at Page 70
#define NVIC_ISER   ((uint32_t*)0xE000E100)   //Interrupt Set-Enable Register page B3-284
#define NVIC_ICER   ((uint32_t*)0xE000E180)   //Interrupt Clear Enable Register page B3-285
#define NVIC_ISPR   ((uint32_t*)0xE000E200)   //Interrupt Set-Pending Register page B3-286
#define NVIC_ICPR   ((uint32_t*)0xE000E280)   //Interrupt Clear-Pending Register page B3-287
#define NVIC_IPRN   ((uint32_t*)0xE000E400)   //-0xE000E43C Interrupt Priority Registers NVIC_IPR0-NVICIPR7 B3-288


#endif 
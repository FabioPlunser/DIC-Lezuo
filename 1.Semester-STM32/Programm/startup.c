#include <stdint.h>

/* STARTUP */
extern void _main(void);

extern uint32_t __bss_begin, __bss_end;
extern uint32_t __data_begin, __data_end;
extern uint32_t __datai_begin, __datai_end;

__attribute__ ((section(".startup"), optimize("-O2"))) void __startup(void) 
{
  uint32_t *bss = &__bss_begin;
  uint32_t *data = &__data_begin;
  uint32_t *datai = &__datai_begin;

  while (datai < &__datai_end)
  {
    *data = *datai;
    data++;
    datai++;
  }

  while (bss < &__bss_end)
  {
    *bss = 0;
    bss++;
  }

  _main();
}

/* weak ISR */
__attribute__ ((section(".isr"), optimize("-O3"))) void __nothing(void) {}
#define __weak_alias_nothing __attribute__ ((weak, alias("__nothing")))

void isr_nmi(void) __weak_alias_nothing;
void isr_hardfault(void) __weak_alias_nothing;
void isr_svcall(void) __weak_alias_nothing;
void isr_pendsv(void) __weak_alias_nothing;
void isr_systick(void) __weak_alias_nothing;

void isr_wwdg(void) __weak_alias_nothing;
void isr_pvd_vddio2(void) __weak_alias_nothing;
void isr_rtc(void) __weak_alias_nothing;
void isr_flash(void) __weak_alias_nothing;
void isr_rcc_crs(void) __weak_alias_nothing;
void isr_exti0_1(void) __weak_alias_nothing;
void isr_exti2_3(void) __weak_alias_nothing;
void isr_exti4_15(void) __weak_alias_nothing;
void isr_tsc(void) __weak_alias_nothing;
void isr_dma_ch1(void) __weak_alias_nothing;
void isr_dma_ch2_3(void) __weak_alias_nothing;
void isr_dma_ch4_5_6_7(void) __weak_alias_nothing;
void isr_adc_comp(void) __weak_alias_nothing;

void isr_tim1_cc(void) __weak_alias_nothing;
void isr_tim2(void) __weak_alias_nothing;
void isr_tim3(void) __weak_alias_nothing;
void isr_tim6_dac(void) __weak_alias_nothing;
void isr_tim7(void) __weak_alias_nothing;
void isr_tim14(void) __weak_alias_nothing;
void isr_tim15(void) __weak_alias_nothing;
void isr_tim16(void) __weak_alias_nothing;
void isr_tim17(void) __weak_alias_nothing;
void isr_i2c1(void) __weak_alias_nothing;
void isr_i2c2(void) __weak_alias_nothing;
void isr_spi1(void) __weak_alias_nothing;
void isr_spi2(void) __weak_alias_nothing;
void isr_usart1(void) __weak_alias_nothing;
void isr_usart2(void) __weak_alias_nothing;
void isr_usart3_4_5_6_7_8(void) __weak_alias_nothing;
void isr_cec_can(void) __weak_alias_nothing;
void isr_usb(void) __weak_alias_nothing;

/* VECTOR TABLE */
struct __vector_table
{
  uint32_t *start_sp;
  void (*sirq[15])(void);
  void (*pirq[32])(void);
};

#define __zero 0x0
extern uint32_t __stack_end;

struct __vector_table vector_table __attribute__ ((section(".vectors"))) =
{
  &__stack_end,
  {
    __startup,
    isr_nmi,
    isr_hardfault,
    __zero,
    __zero,
    __zero,
    __zero,
    __zero,
    __zero,
    __zero,
    isr_svcall,
    __zero,
    __zero,
    isr_pendsv,
    isr_systick,
  },
  {
    isr_wwdg,
    isr_pvd_vddio2,
    isr_rtc,
    isr_flash,
    isr_rcc_crs,
    isr_exti0_1,
    isr_exti2_3,
    isr_exti4_15,
    isr_tsc,
    isr_dma_ch1,
    isr_dma_ch2_3,
    isr_dma_ch4_5_6_7,
    isr_adc_comp,
    __zero,
    isr_tim1_cc,
    isr_tim2,
    isr_tim3,
    isr_tim6_dac,
    isr_tim7,
    isr_tim14,
    isr_tim15,
    isr_tim16,
    isr_tim17,
    isr_i2c1,
    isr_i2c2,
    isr_spi1,
    isr_spi2,
    isr_usart1,
    isr_usart2,
    isr_usart3_4_5_6_7_8,
    isr_cec_can,
    isr_usb
  }
};
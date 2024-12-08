#include "stm32f1xx.h"

void SetSysClockTo72();
void PINC_13_INIT();

int main()
{
  SetSysClockTo72();
  PINC_13_INIT();
  

  
  while(1){
    GPIOC->BSRR|=GPIO_BSRR_BS13;//OFF PC13 LED
    for(uint32_t i=0;i<450000;i++);
    GPIOC->BSRR|=GPIO_BSRR_BR13;//On PC13LED
    for(uint32_t i=0;i<450000;i++);
  }
  return 0;
}
void PINC_13_INIT()
{
  RCC->APB2ENR|=RCC_APB2ENR_IOPCEN;//enabling port clocking C
  
  GPIOC->CRH&=~GPIO_CRH_MODE13_0;
  GPIOC->CRH|=GPIO_CRH_MODE13_1;
  
  GPIOC->CRH&=~GPIO_CRH_CNF13_0;//Mode port 13 OUTPUT 
  GPIOC->CRH&=~GPIO_CRH_CNF13_1;//Spead 2MHz
  
}
void SetSysClockTo72()
{
  RCC->CR|=RCC_CR_HSEON;//enable external timing 
  
  while(READ_BIT(RCC->CR,RCC_CR_HSERDY==RESET));
  //we are waiting for the clock to stabilize
  
  
  FLASH->ACR&=~FLASH_ACR_PRFTBE;
  FLASH->ACR|=FLASH_ACR_PRFTBE;
  
  FLASH->ACR&=~FLASH_ACR_LATENCY;
  FLASH->ACR|=FLASH_ACR_LATENCY_2;
  
  RCC->CFGR&=~RCC_CFGR_HPRE;
  RCC->CFGR|=RCC_CFGR_HPRE_DIV1;
  //setting the division SYSCL to 1
  
  RCC->CFGR&=~RCC_CFGR_PPRE2;
  RCC->CFGR|=RCC_CFGR_PPRE2_DIV1;
  //setting the division APB2 prescaler to 2
  
  RCC->CFGR&=~RCC_CFGR_PPRE1;
  RCC->CFGR|=RCC_CFGR_PPRE1_DIV2;
  //setting the division APB1 prescaler to 2
  
  RCC->CFGR&=(uint32_t)((uint32_t)~(RCC_CFGR_PLLSRC|RCC_CFGR_PLLXTPRE|RCC_CFGR_PLLMULL));
  RCC->CFGR|=(uint32_t)(RCC_CFGR_PLLSRC|RCC_CFGR_PLLMULL9);
  //select the HSE input in the multiplexer instead of HSI/2
  //setting the frequency multiplication to 9
  
  RCC->CR|=RCC_CR_PLLON;//turning on the PLL
  
  while(READ_BIT(RCC->CR,RCC_CR_PLLRDY)!=(RCC_CR_PLLRDY));
  
  RCC->CFGR&=~RCC_CFGR_SW;
  RCC->CFGR|=RCC_CFGR_SW_PLL;
  //we select PLLCLK in the multiplexer
  
  while(READ_BIT(RCC->CFGR,RCC_CFGR_SWS)!=RCC_CFGR_SWS_PLL);
  //we are waiting for the PLL to become a clock source
  
}
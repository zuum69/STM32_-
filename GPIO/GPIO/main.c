#include "stm32f1xx.h"
#define LED_PINC13_ON GPIOC->BSRR|=GPIO_BSRR_BR13
#define LED_PINC13_OFF GPIOC->BSRR|=GPIO_BSRR_BS13
#define GET_PIN_A0 READ_BIT(GPIOA->IDR,GPIO_IDR_IDR0)

void SetSysClockTo72();
void PINC_13_INIT_OUTPUT();
void PINA_0_INIT_INPUT();
void Interrapt_EXTI_PR_PR0();

int main()
{
  SetSysClockTo72();
  PINC_13_INIT_OUTPUT();
  PINA_0_INIT_INPUT();
  Interrapt_EXTI_PR_PR0();

  
  while(1){
    
  }
  return 0;
}
void EXTI0_IRQHandler()
{
  EXTI->PR|=EXTI_PR_PR0;
  if(READ_BIT(EXTI->PR,EXTI_PR_PR0))
  {
    LED_PINC13_ON;
    for(uint32_t i=0;i<4500000;i++);
    LED_PINC13_OFF;
  }
}
void Interrapt_EXTI_PR_PR0()
{
  EXTI->PR|=EXTI_PR_PR0;//Resetting the interrupt flag
 
  EXTI->IMR|=EXTI_IMR_MR0;//Enabling interrupts for channels 0
 
  AFIO->EXTICR[0]&=~AFIO_EXTICR1_EXTI0_PA;//Selecting the PA0 port to interrupt
 
  RCC->APB2ENR|=RCC_APB2ENR_IOPAEN;//Enabling the clocking of port A
 
  EXTI->FTSR|=EXTI_FTSR_TR0;//Interruption due to power loss
 
  NVIC_EnableIRQ(EXTI0_IRQn);//Interrupt resolution
 
  NVIC_SetPriority(EXTI0_IRQn,0);//Setting the interrupt priority
}
void PINA_0_INIT_INPUT()
{
  RCC->APB2ENR|=RCC_APB2ENR_IOPAEN;////enabling port clocking A
  
  GPIOA->CRL&=~GPIO_CRL_MODE0_0;
  GPIOA->CRL&=~GPIO_CRL_MODE0_1;//INPUT
  
  GPIOA->CRL&=~GPIO_CRL_CNF0_0;
  GPIOA->CRL|=GPIO_CRL_CNF0_1;//Input with disconnected resistors
  GPIOA->BSRR|=GPIO_BSRR_BR0;
}
void PINC_13_INIT_OUTPUT()
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
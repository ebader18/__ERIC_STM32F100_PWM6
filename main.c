#include "stm32f10x.h"

void Init_6CHPWM();

int main(void)
{
  uint32_t count;
  RCC->APB2ENR = RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPBEN | RCC_APB2ENR_IOPCEN;     // Enable all GPIO Clock & Alternate Function Clock*//*Enable all GPIO Clock & Alternate Function Clock
  RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;   // Enable TIMER1 & USART1 Clock

  GPIOB->CRL = 0x444444AA;              // Configure PB0 & PB1 as Alternate Function Push Pull Output Max Speed = 2MHz
  GPIOB->CRH = 0x44444444;              // Default value => Floating input
  GPIOA->CRL = 0xA4444444;              // PA7 as Alternate Function Push Pull Output Max Speed = 2MHz
  GPIOA->CRH = 0x44444AAA;              // PA8 - PA10 as Alternate Function Push Pull Output Max Speed = 2MHz
  GPIOC->CRL = 0x44444442;              // PC0 as General Purpose Push Pull Output Max Speed = 2MHz
  GPIOC->CRH = 0x44444444;              // Default value => Floating input

  /* Partial remap (ETR/PA12, CH1/PA8, CH2/PA9, CH3/PA10, CH4/PA11, BKIN/PA6, CH1N/PA7, CH2N/PB0, CH3N/PB1) */
  /* CH1: PA8 / PA7 */
  /* CH2: PA9 / PB0 */
  /* CH3: PA10 / PB1 */
  AFIO->MAPR |= 0x00000040;

  Init_6CHPWM();                        //Initialize 20KHz PWM on CH 1-3
  
  count = 0;
  TIM1->CR1 |= 0x00000001;            // Counter Enable
  while(1)
  {
    if (count++ >= SystemCoreClock/100)
    {
      count = 0;
      GPIOC->ODR ^= 1 << 0;             // Toggle a single bit
      if (TIM1->CCR1 >= 1900)
        TIM1->CCR1 = 100;
      else
        TIM1->CCR1 = TIM1->CCR1 + 100;
      //GPIOC->ODR |= 1 << 0;           // Set a single bit
      //GPIOC->ODR &= ~(1 << 0);        // Clear a single bit
    }
  }
}

void Init_6CHPWM()
{
    TIM1->CCMR1 |= 0x00006868;          // CH1 & CH2 Output Compare, Preload, PWM1 Enable
    TIM1->CCMR2 |= 0x00000068;          // CH3 Output Compare, Preload, PWM1 Enable
    TIM1->CCER  |= 0x00000555;          // CH1, CH2, CH3 and their Corresponding  Complement Output Enable
    TIM1->CR1 |= 0x000000A0;            // Centre Align Mode 1 & Auto-reload Preload enable
    TIM1->PSC = 0;                      // Prescaler set to 0
    TIM1->ARR = (2000 - 1);             // Auto reload value 1400 (Period = 50us)
    TIM1->CCR1 = 100;                   // Start PWM duty for channel 1
    TIM1->CCR2 = 500;                   // Start PWM duty for channel 2
    TIM1->CCR3 = 350;                   // Start PWM duty for channel 3
    TIM1->BDTR |= 0x00008800;           // Enable Main O/P & Enable OSSR
    //TIM1->CR1 |= 0x00000001;            // Counter Enable
}
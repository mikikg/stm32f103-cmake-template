#include "stm32f1xx.h"

void MYSYS_INIT(void) {
    /***********    ----    Clock Setup     -----   ***********/
    //  HSE -> PLL -> SYSCLK
    //  8MHz    *9    72MHz
    //
    RCC->CR |= (1 << 16);            // Enable HSE
    while(!(RCC->CR & (1 << 17)));   // Wait till HSE ready
    RCC->CFGR |= 1;                  // Swich to HSE temporarly
    RCC->CR   &= ~1;                 // Disable HSI
    RCC->CFGR |= (0b111 << 18);      // Set PLL multiplication to 9
    RCC->CFGR |= (1 << 16);          // HSE as PLL_SRC
    FLASH->ACR = 0b10010;            // Enable flash prefetch
    RCC->CR |= (1 << 24);            // Enable PLL
    while(!(RCC->CR & (1 << 25)));   // Wait till PLL ready
    RCC->CFGR = (RCC->CFGR | 0b10) & ~1; // Set PLL as Clock source
    while(!(RCC->CFGR & (1 << 3)));  // Wait till PLL is CLK SRC
    /***********    ----    Clock ready     -----   ***********/
    
    //ukljuci clock za APB2, on je za sve GPIOx
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN; //GPIOA
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN; //GPIOB
    RCC->APB2ENR |= RCC_APB2ENR_IOPCEN; //GPIOC
    
    //Alternate Function I/O clock enable - Treba da bi proradile GPIO na JTAG, EXTI, I2C
    RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
    
    //oslobodi nozice od JTAG, koristimo samo SWD
    AFIO->MAPR |= AFIO_MAPR_SWJ_CFG_JTAGDISABLE; //JTAG-DP Disabled and SW-DP Enabled
    
    //konfiguracija za PC13
    //po default svi pinovi su Input
    //ima dva registra CRL za pinove 0-7 i CRH za pinove 8-15
    //definisi pin kao general purpose Output push-pull, 50MHz
    GPIOC->CRH &= ~GPIO_CRH_CNF13_0; //ocisti bit CNF13_0
    GPIOC->CRH &= ~GPIO_CRH_CNF13_1; //ocisti bit CNF13_1
    GPIOC->CRH |= GPIO_CRH_MODE13_0; //setuj bit MODE13_0
    GPIOC->CRH |= GPIO_CRH_MODE13_1; //setuj bit MODE13_1
}

//----------------------------------------
// Main
//----------------------------------------
int main(void) {
    
    MYSYS_INIT();
    
    //Blink led
    while (1){
        for (int x = 0; x < 300000; x++) {GPIOC->BSRR=GPIO_BSRR_BS13;}
        for (int x = 0; x < 300000; x++) {GPIOC->BSRR=GPIO_BSRR_BR13;}
    }
    
}

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <sys/unistd.h>
#include "stm32f401xe.h"

void toggle(void) {
    GPIOA->ODR ^= (0x1 << 5);
}

void led5_setup() {
    // IO port A clock enable
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;

    // GPIO port A mode register
    GPIOA->MODER |= GPIO_MODER_MODER5_0;

    // GPIO port A output speed register
    GPIOA->OSPEEDR |= GPIO_MODER_MODER5_Msk;

    // GPIO port pull-up/pull-down register
    GPIOA->PUPDR |= GPIO_PUPDR_PUPD5_1;
}

void usart1_setup(void) {
    // USART2 clock enable
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
    // IO port A clock enable
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;

    /**
     * Set PA9 and PA10 to 'Alternate function mode'
     * 
     * See Reference Manual 8.4.1 GPIO port mode register for more details
     */
    GPIOA->MODER |= (0x2 << (9 * 2));  // PA9 TX
    GPIOA->MODER |= (0x2 << (10 * 2)); // PA10 RX

    /**
     * Set the actual alternative function mode to AF7 (see Reference Manuel Figure 17)
     * 
     * See Reference Manual 8.4.9 GPIO alternate function low register for more details
     */
    GPIOA->AFR[1] |= (0x7 << (1 * 4)); // PA9 to AF7
    GPIOA->AFR[1] |= (0x7 << (2 * 4)); // PA10 to AF7

    /**
     * Set baudrate to 9600 on 16 Mhz
     * 
     * See Reference Manual '19.3.4 Fractional baud rate generation' and '19.6.3 Baud rate register (USART_BRR)' for more details
     */
    // USART1->BRR |= 0x0003; // DIV Fraction 9600 baud
    // USART1->BRR |= 0x0680; // DIV Mantissa 9600 baud

    // USART1->BRR = 0x8B; // 115200
    USART1->BRR |= 0x000B; // DIV Fraction 115200 baud
    USART1->BRR |= 0x0080; // DIV Mantissa 115200 baud

    /**
     * Enable transmit and receive
     */
    USART1->CR1 |= USART_CR1_TE;
    USART1->CR1 |= USART_CR1_RE;

    /**
     * 
     */
    USART1->CR1 |= USART_CR1_RXNEIE;

    /**
     * Enable USART1
     */
    USART1->CR1 |= USART_CR1_UE;

    NVIC_EnableIRQ(USART1_IRQn);
}

void usart2_setup(void) {
    // USART2 clock enable
    RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
    // IO port A clock enable
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;

    /**
     * Set PA2 and PA3 to 'Alternate function mode'
     * 
     * See Reference Manual 8.4.1 GPIO port mode register for more details
     */
    GPIOA->MODER |= (0x2 << (2 * 2)); // PA2 TX
    GPIOA->MODER |= (0x2 << (3 * 2)); // PA3 RX

    /**
     * Set the actual alternative function mode to AF7 (see Reference Manuel Figure 17)
     * 
     * See Reference Manual 8.4.9 GPIO alternate function low register for more details
     */
    GPIOA->AFR[0] |= (0x7 << (2 * 4)); // PA2 to AF7
    GPIOA->AFR[0] |= (0x7 << (3 * 4)); // PA3 to AF7

    /**
     * Set baudrate to 9600 on 16 Mhz
     * 
     * See Reference Manual '19.3.4 Fractional baud rate generation' and '19.6.3 Baud rate register (USART_BRR)' for more details
     */
    // USART2->BRR |= 0x0003; // DIV Fraction
    // USART2->BRR |= 0x0680; // DIV Mantissa

    USART2->BRR |= 0x000B; // DIV Fraction 115200 baud
    USART2->BRR |= 0x0080; // DIV Mantissa 115200 baud

    /**
     * Enable transmit and receive
     */
    USART2->CR1 |= USART_CR1_TE;
    USART2->CR1 |= USART_CR1_RE;

    /**
     * 
     */
    USART2->CR1 |= USART_CR1_RXNEIE;

    /**
     * Enable USART2
     */
    USART2->CR1 |= USART_CR1_UE;

    NVIC_EnableIRQ(USART2_IRQn);
}

/**
 * Constructor functions run before main()
 */
__attribute__((constructor))
void init_func(void) {
    led5_setup();
    usart1_setup();
    usart2_setup();
}

void delay(void) {
    for(int i = 0; i < 1000000; i++) {
        __asm__("nop");
    }
}

void usrt1_write(int character) {
    // wait for Transmit data register to be cleared
    while (!(USART1->SR & USART_SR_TXE));
    USART1->DR = (character & 0xFF);
}

void usrt2_write(int character) {
    // wait for Transmit data register to be cleared
    while (!(USART2->SR & USART_SR_TXE));
    USART2->DR = (character & 0xFF);
}

void isr_usart1(void) {
    if (USART1->SR & USART_SR_RXNE) {
        // read the actual data from the data register. This also clears the RXNE flag
        char received_char = USART1->DR;

        // transmit value as is via UART
        usrt2_write(received_char);
    }
}

void isr_usart2(void) {
    if (USART2->SR & USART_SR_RXNE) {
        // read the actual data from the data register. This also clears the RXNE flag
        char received_char = USART2->DR;

        // transmit value as is via UART
        usrt1_write(received_char);
    }
}

char usart1_read(void) {
    // wait for Read data register not empty to be cleared
    while (!(USART1->SR & USART_SR_RXNE));
    return USART1->DR & 0xFF;
}

char usart2_read(void) {
    // wait for Read data register not empty to be cleared
    while (!(USART2->SR & USART_SR_RXNE));
    return USART2->DR & 0xFF;
}

/**
 * _write - Redirects standard output or error to USART2.
 * 
 * This function is used by the `printf` family of functions to output data.
 * It is typically called by the Newlib C library when `printf` or similar 
 * functions are used in an embedded environment.
 * 
 * Implementation details:
 * If the file descriptor is either STDOUT_FILENO or STDERR_FILENO,
 * the function iterates over the data buffer and sends each character 
 * to USART2 using the `usrt2_write` function. This effectively redirects 
 * the output from `printf` to a UART interface, allowing debug prints 
 * to be seen over a serial connection.
 */
int _write(int file, char *data, int len) {
    if (file == STDOUT_FILENO || file == STDERR_FILENO) {
        for (int i = 0; i < len; i++) {
            usrt2_write(data[i]);
        }
        return len;
    }
    return -1;
}

int main(void) {
    printf("Hello World\r\n");
    
    /* Loop forever */
    for(;;) {
        toggle();
        delay();
    }
    return 0;
}

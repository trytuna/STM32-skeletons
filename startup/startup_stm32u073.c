#include <stdint.h>
#include <string.h>

/**
 * newlib-nano is going to spit out warnings like the following while linking. In order 
 * to suppress these them we need to stubs.
 * 
 * warning: _close is not implemented and will always fail
 */
void _close(void) {}
void _fstat(void) {}
void _isatty(void) {}
void _lseek(void) {}
void _read(void) {}

extern void (*_init_array_start[])(void);
extern void (*_init_array_end[])(void);

/**
 * @brief Calls all initialization functions in the .init_array section.
 *
 * This function is invoked during the startup phase to execute all functions
 * stored between '_init_array_start' and '_init_array_end'. It iterates over
 * the array of function pointers and calls each one in order, ensuring that all
 * static constructors and initialization routines are executed before the main
 * program begins.
 */
void init_array(void) {
    size_t count = _init_array_end - _init_array_start;
    for (size_t i = 0; i < count; i++) {
        _init_array_start[i]();
    }
}

extern size_t _data_load; // load address in flash
extern size_t _data_start; // start address in ram
extern size_t _data_end; // end address in ram

/**
 * @brief Copies initialized data from flash memory to RAM.
 *
 * This function is called during the startup phase to copy the initialized
 * global and static variables from non-volatile flash memory to volatile RAM.
 * It iterates over the range from '_data_start' to '_data_end', transferring
 * data from the load address '_data_load' to ensure all variables have their
 * defined initial values in RAM before the program execution continues.
 */
void copy_data_section(void) {
    size_t *src = &_data_load;
    size_t *dst = &_data_start;
    
    while (dst < &_data_end) {
        *dst++ = *src++;
    }
}

extern size_t _bss_start;
extern size_t _bss_end;

/**
 * @brief Clears the .bss section by setting it to zero.
 *
 * This function is called during the startup phase to initialize
 * the .bss section, which contains uninitialized global and static variables.
 * It sets all bytes in the .bss section to zero to ensure that these variables
 * have a known starting value before the program execution continues.
 */
void clear_bss_section(void) {
    uint8_t *bss_ptr = (uint8_t *)&_bss_start;
    size_t size = (size_t)&_bss_end - (size_t)&_bss_start;

    while (size--) {
        *bss_ptr++ = 0;
    }
}
// // Alternative to the function above but requires linking to a standard C library like newlib, newlib-nano or picolibc
// void clear_bss_section(void) {
//     memset(&_bss_start, 0, (size_t)&_bss_end - (size_t)&_bss_start);
// }

extern int main(void);

void isr_reset(void) {
    copy_data_section();
    clear_bss_section();
    init_array();

    main();

    // `main()` should never return. In any case we loop here forever. This mitigates unexpected behaviour.
    while(1);
}

/**
 * @brief ISR for the HardFault exception.
 *
 * This function is called when a HardFault exception occurs, which typically indicates
 * a critical error such as an invalid memory access, division by zero, or execution of
 * an undefined instruction. The default implementation enters an infinite loop, but you
 * can customize it to log error information, reset the system, or attempt recovery.
 */
void isr_hardfault(void) {
    /**
     * TODO:
     *   - Update to Cortex M0
     * 
     * Further information about the SCB (System control block) is described in the Programming Manual
     * in chapter 4.4 System control block (SCB).
     * 
     * https://www.st.com/resource/en/programming_manual/pm0223-stm32-cortexm0-mcus-programming-manual-stmicroelectronics.pdf
     */
    // volatile uint32_t scb_hfsr = 0xE000ED2C; /* HardFault Status Register */
    // volatile uint32_t scb_cfsr = 0xE000ED28; /* Configurable Fault Status Register */
    // volatile uint32_t scb_mmsr = 0xE000ED28; /* MemManage Fault Address Register */
    // volatile uint32_t scb_bfsr = 0xE000ED29; /* BusFault Address Register */

    /* Implement your error handling or logging here */
    // For example, you might log the fault information to a debug console
    // or store it in a non-volatile memory for later analysis.

    /* Infinite loop to halt the system */
    while (1) {
        /* You can also toggle an LED here to indicate a HardFault has occurred */
    }
}

void isr_default(void) {
    /* Infinite loop to indicate unhandled interrupt */
    while (1) {
        /* Optional: Toggle an LED here for debugging */
    }
}

/**
 * A full list of the vector table can be found in the Reference Manual
 * in table 54 Vector table.
 * 
 * Note that there are several reserved entries that are not part of the table. These entries can be identified by an interruption in the
 * sequential numbering.
 * 
 * https://www.st.com/resource/en/reference_manual/rm0503-stm32u0-series-advanced-armbased-32bit-mcus-stmicroelectronics.pdf
 */
#define IVT_ARRAY_SIZE (48U)

typedef void (*isr_t)(void);
extern size_t _stack;
extern void isr_usart1(void);
extern void isr_usart2(void);

__attribute__((used, section(".ivt")))
static const isr_t ivt[IVT_ARRAY_SIZE] = {
    (isr_t)&_stack,         // Initial SP value
    isr_reset,              // Reset
    isr_default,            // NMI
    isr_hardfault,          // HardFault
    0, 0, 0, 0, 0, 0, 0,    // Reserved
    isr_default,            // SVC
    0, 0,                   // Reserved
    isr_default,            // PendSV
    isr_default,            // SysTick
    isr_default,            // WWDG
    isr_default,            // PVD_PVM
    isr_default,            // RTC / TAMP
    isr_default,            // FLASH
    isr_default,            // RCC / CRS
    isr_default,            // EXTI0_1
    isr_default,            // EXTI2_3
    isr_default,            // EXTI4_15
    isr_default,            // USB
    isr_default,            // DMA1_Channel1
    isr_default,            // DMA1_Channel2_3
    isr_default,            // DMA1_Channel4_5_6_7 / DMAMUX / DMA2_Channel1_2_3_4_5
    isr_default,            // ADC_COMP
    isr_default,            // TIM1_BRK_UP_TRG_COM
    isr_default,            // TIM1_CC
    isr_default,            // TIM2
    isr_default,            // TIM3
    isr_default,            // TIM6_DAC / LPTIM1
    isr_default,            // TIM7 / LPTIM2
    isr_default,            // TIM15 / LPTIM3
    isr_default,            // TIM16
    isr_default,            // TSC
    isr_default,            // LCD
    isr_default,            // I2C1
    isr_default,            // I2C2 / I2C3 / I2C4
    isr_default,            // SPI1
    isr_default,            // SPI2 / SPI3
    isr_default,            // USART1
    isr_default,            // USART2 / LPUART2
    isr_default,            // USART3 / LPUART1
    isr_default,            // USART4 / LPUART3
    isr_default,            // AES / RNG
};

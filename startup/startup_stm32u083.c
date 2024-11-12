extern int main(void);
extern unsigned int _stack;

void isr_reset(void) {
    main();

    // `main()` should never return. In case it does it nontheless we loop here forever. This mitigates unexpected behaviour.
    while(1);
}

void isr_hardfault(void) {
    while(1);
}

// https://www.st.com/resource/en/programming_manual/pm0223-stm32-cortexm0-mcus-programming-manual-stmicroelectronics.pdf (Figure 8. Vector table)
#define IVT_ARRAY_SIZE (48U)

typedef void (*isr_t)(void);

__attribute((used, section(".ivt")))
static const isr_t ivt[IVT_ARRAY_SIZE] = {
    (isr_t)&_stack,         // Initial SP value
    isr_reset,              // Reset
    0,                      // NMI
    isr_hardfault,          // HardFault
    0, 0, 0, 0, 0, 0, 0,    // Reserved
    0,                      // SVCall
    0, 0,                   // Reserved
    0,                      // PendSV
    0,                      // SysTick
    0, 0, 0, 0, 0, 0, 0, 0, // IRQ0-7
    0, 0, 0, 0, 0, 0, 0, 0, // IRQ8-15
    0, 0, 0, 0, 0, 0, 0, 0, // IRQ16-23
    0, 0, 0, 0, 0, 0, 0, 0, // IRQ24-31
};

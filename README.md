## Compiling
    /Applications/ArmGNUToolchain/13.3.rel1/arm-none-eabi/bin/arm-none-eabi-gcc -mcpu=cortex-m0 -mthumb -g -O0 -c main.c -o main.o
    /Applications/ArmGNUToolchain/13.3.rel1/arm-none-eabi/bin/arm-none-eabi-gcc -mcpu=cortex-m0 -mthumb -g -O0 -c startup_stm32u083.c -o startup_stm32u083.o
    /Applications/ArmGNUToolchain/13.3.rel1/arm-none-eabi/bin/arm-none-eabi-gcc -mcpu=cortex-m0 -mthumb -g -O0 -c startup_stm32f401re.c -o startup_stm32f401re.o

## Linking
    /Applications/ArmGNUToolchain/13.3.rel1/arm-none-eabi/bin/arm-none-eabi-gcc -nostdlib main.o startup_stm32u083.o -o firmware.elf -T stm32u083.ld -Wl,--verbose -Wl,-Map=firmware.map
    /Applications/ArmGNUToolchain/13.3.rel1/arm-none-eabi/bin/arm-none-eabi-gcc -nostdlib main.o startup_stm32f401re.o -o firmware.elf -T stm32f401re.ld -Wl,--verbose -Wl,-Map=firmware.map

### Link with newlib-nano

    /Applications/ArmGNUToolchain/13.3.rel1/arm-none-eabi/bin/arm-none-eabi-gcc -nostartfiles --specs=nosys.specs --specs=nano.specs -mcpu=cortex-m0 -mthumb main.o startup_stm32f401re.o -o firmware.elf -T stm32f401re.ld -Wl,--verbose

## Debug
    /Applications/ArmGNUToolchain/13.3.rel1/arm-none-eabi/bin/arm-none-eabi-size main.o
    /Applications/ArmGNUToolchain/13.3.rel1/arm-none-eabi/bin/arm-none-eabi-objdump -d main.o

## Flashing

    st-flash write firmware.elf 0x08000000

## Download SVD Files

https://www.st.com/en/microcontrollers-microprocessors/stm32-32-bit-arm-cortex-mcus.html#cad-resources
https://stm32-rs.github.io/stm32-rs/

## Install CMSIS Toolbox

https://github.com/Open-CMSIS-Pack/cmsis-toolbox

## Download CMSIS 6

https://github.com/ARM-software/CMSIS_6

## Download CMSIS Device

https://github.com/STMicroelectronics/cmsis-device-f4
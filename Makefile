SRC_DIR := src
SRC_FILES := $(wildcard $(SRC_DIR)/*.c)
OUTPUT_DIR := .build
OBJ_FILES := $(patsubst $(SRC_DIR)/%.c, $(OUTPUT_DIR)/%.o, $(SRC_FILES))

MCUFLAGS := -mcpu=cortex-m4 -mthumb
CFLAGS := -ffunction-sections -fdata-sections -g -O0 -ICMSIS/Core/Include -Icmsis-device-f4-master/Include
LDFLAGS := -nostartfiles --specs=nano.specs --specs=nosys.specs -Wl,--gc-sections -Wl,-Map=$(OUTPUT_DIR)/firmware.map -Wl,--print-memory-usage
# LDFLAGS += -Wl,--verbose

TOOLCHAIN := /Applications/ArmGNUToolchain/13.3.rel1/arm-none-eabi
TOOLCHAIN_BIN := $(TOOLCHAIN)/bin
CC := $(TOOLCHAIN_BIN)/arm-none-eabi-gcc
OBJCOPY := $(TOOLCHAIN_BIN)/arm-none-eabi-objcopy
OPENOCD := openocd

# STARTUP_FILE := startup_stm32u083.c
STARTUP_FILE := startup_stm32f401re.c
# LINK_SCRIPT := stm32u083.ld
LINK_SCRIPT := stm32f401re.ld


# https://www.gnu.org/software/make/manual/html_node/Phony-Targets.html
.PHONY: all flash-stlink flash clean

all: .build/startup.o firmware.elf firmware.bin

# https://www.gnu.org/software/make/manual/html_node/Prerequisite-Types.html
$(OUTPUT_DIR):
	@mkdir -p $(OUTPUT_DIR)

$(OUTPUT_DIR)/startup.o: ./startup/$(STARTUP_FILE) | $(OUTPUT_DIR)
	@$(CC) $(MCUFLAGS) $(CFLAGS) -c $< -o $@

$(OUTPUT_DIR)/%.o: $(SRC_DIR)/%.c | $(OUTPUT_DIR)
	@$(CC) $(MCUFLAGS) $(CFLAGS) -c $< -o $@

firmware.elf: $(OBJ_FILES) $(OUTPUT_DIR)/startup.o
	@$(CC) -T $(LINK_SCRIPT) $(LDFLAGS) $(MCUFLAGS) $(OUTPUT_DIR)/startup.o $(OBJ_FILES) -o $(OUTPUT_DIR)/firmware.elf

firmware.bin: firmware.elf
	@$(OBJCOPY) -O binary $(OUTPUT_DIR)/firmware.elf $(OUTPUT_DIR)/firmware.bin

flash-stlink:
	st-flash write $(OUTPUT_DIR)/firmware.bin 0x8000000

flash:
	$(OPENOCD) -f interface/stlink.cfg -f target/stm32f4x.cfg -c "program $(OUTPUT_DIR)/firmware.elf verify reset exit"

# https://www.gnu.org/software/make/manual/html_node/Echoing.html
# https://www.gnu.org/software/make/manual/html_node/Errors.html
clean:
	@-rm -rf $(OUTPUT_DIR)

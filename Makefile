# ==============================================================================
# HARDWARE CONFIGURATION AND TOOLCHAIN DEFINITIONS
# ==============================================================================
MCU         := atmega328p
F_CPU       := 8000000UL
F_CLOCK_REF := 1000UL
PROGRAMMER  := usbasp

# AVR Toolchain Executables
CC          := avr-gcc
OBJCOPY     := avr-objcopy
OBJDUMP     := avr-objdump  # Optional 
AVRDUDE     := avrdude
SIZE        := avr-size

# Project Directories & Target Name
BUILD_DIR   := build
TARGET      := $(BUILD_DIR)/firmware

# ==============================================================================
# AUTOMATIC SOURCE AND INCLUDE DETECTION
# ==============================================================================
# Find all C source files in main/ and components/*/
SRCS        := $(wildcard main/*.c) $(wildcard HAL/*/*.c)        # .c in components are not deteccted only in components/*/

# Automatically find all include directories under components/
INC_DIRS    := $(wildcard HAL/*/include)				# if .c in components direcctly add acces to include in component here
INC_FLAGS   := $(addprefix -I,$(INC_DIRS))				# #include "lcd.h" in main with -I prefix 					
									# (without : #include "../../components/display/include/lcd.h")

# Map source files (.c) to object files (.o) inside the build/ directory
OBJS        := $(patsubst %.c, $(BUILD_DIR)/%.o, $(SRCS))		# Prepare the directory (in build)/name for object file(.o)

# ==============================================================================
# COMPILER AND LINKER FLAGS
# ==============================================================================
# C Flags: Optimization (-Os), MCU definition, CPU frequency, All Warnings
CFLAGS      := -mmcu=$(MCU) -DF_CPU=$(F_CPU) -Os -Wall -Wextra -std=gnu99 $(INC_FLAGS)		# Config for complilation (avr-gcc) (MCU,Freq,Opti Size,Warning,C99 Standard,Direcctory to .h)

# Linker Flags
LDFLAGS     := -mmcu=$(MCU)						# Config for linker (avr-gcc)

# ==============================================================================
# BUILD RULES
# ==============================================================================
.PHONY: all flash clean size help fuses-read fuses-8mhz

# Default target: build the binary
all: $(TARGET).hex size							# Build (include print memory usage)

# Rule to generate the INTEL HEX file from ELF binary
$(TARGET).hex: $(TARGET).elf
	@echo "Creating HEX file for flashing..."
	@$(OBJCOPY) -O ihex -R .eeprom $< $@

# Rule to link object files into ELF binary
$(TARGET).elf: $(OBJS)
	@echo "Linking object files into $@"
	@mkdir -p $(dir $@)
	@$(CC) $(LDFLAGS) $^ -o $@

# Pattern rule to compile .c files into .o object files
$(BUILD_DIR)/%.o: %.c
	@echo "Compiling $<..."
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) -c $< -o $@

# Display memory usage (RAM / Flash)
size: $(TARGET).elf
	@echo "\n--- Memory Usage ---"
	@$(SIZE) --format=avr --mcu=$(MCU) $<

# ==============================================================================
# FUSE MANAGEMENT (USBasp)
# ==============================================================================
# Read current fuses configuration
fuses-read:
	$(AVRDUDE) -c $(PROGRAMMER) -p $(MCU) -U lfuse:r:-:h -U hfuse:r:-:h -U efuse:r:-:h

# Set 8 MHz internal RC oscillator without CKDIV8 (Low Fuse = 0xE2)
fuses-8mhz:
	$(AVRDUDE) -c $(PROGRAMMER) -p $(MCU) -U lfuse:w:0xe2:m

# ==============================================================================
# UTILITY RULES
# ==============================================================================
# Flash the firmware 
flash: $(TARGET).hex
	@echo "Flashing device via $(PROGRAMMER)..."
	$(AVRDUDE) -c $(PROGRAMMER) -p $(MCU) -U flash:w:$<:i

# Clean all generated build artifacts
clean:
	@echo "Cleaning build directory..."
	@rm -rf $(BUILD_DIR)

# Display help information
help:
	@echo "Available targets:"
	@echo "  all         - Build the entire project (ELF and HEX)"
	@echo "  flash       - Program the target microcontoller via USBasp"
	@echo "  fuses-read  - Read current MCU fuses"
	@echo "  fuses-8mhz  - Configure MCU for 8MHz internal RC (no CKDIV8)"
	@echo "  clean       - Remove all compiled artifacts"
	@echo "  size        - Display code size (Flash and SRAM usage)"
################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include Drivers/STM32F4xx_HAL_Driver/Src/subdir.mk
-include Core/driver/src/subdir.mk
-include Core/driver/cfg/subdir.mk
-include Core/Startup/subdir.mk
-include Core/Src/subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

OPTIONAL_TOOL_DEPS := \
$(wildcard ../makefile.defs) \
$(wildcard ../makefile.init) \
$(wildcard ../makefile.targets) \


BUILD_ARTIFACT_NAME := GOS2022_bootloader
BUILD_ARTIFACT_EXTENSION := elf
BUILD_ARTIFACT_PREFIX :=
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME)$(if $(BUILD_ARTIFACT_EXTENSION),.$(BUILD_ARTIFACT_EXTENSION),)

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
GOS2022_bootloader.elf \

MAP_FILES += \
GOS2022_bootloader.map \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
GOS2022_bootloader.list \

OBJCOPY_BIN += \
GOS2022_bootloader.bin \


# All Target
all: main-build

# Main-build Target
main-build: GOS2022_bootloader.elf secondary-outputs

dependents:
	-cd C:/Users/Gabor/Documents/GitHub/GOS2022_root/Projects/Bootloader/GOS2022_lib/Lib_for_bootloader && $(MAKE) clean all
	-cd C:/Users/Gabor/Documents/GitHub/GOS2022_root/Projects/Bootloader/GOS2022/GOS2022_bootloader && $(MAKE) clean all

# Tool invocations
GOS2022_bootloader.elf GOS2022_bootloader.map: $(OBJS) $(USER_OBJS) C:/Users/Gabor/Documents/GitHub/GOS2022_root/Projects/Bootloader/GOS2022_lib/Lib_for_bootloader/libGOS2022_lib.a C:/Users/Gabor/Documents/GitHub/GOS2022_root/Projects/Bootloader/GOS2022/GOS2022_bootloader/libGOS2022.a C:\Users\Gabor\Documents\GitHub\GOS2022_root\Projects\Bootloader\GOS2022_bootloader\STM32F446RETX_FLASH.ld makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-gcc -o "GOS2022_bootloader.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m4 -T"C:\Users\Gabor\Documents\GitHub\GOS2022_root\Projects\Bootloader\GOS2022_bootloader\STM32F446RETX_FLASH.ld" --specs=nosys.specs -Wl,-Map="GOS2022_bootloader.map" -Wl,--gc-sections -static -L"C:\Users\Gabor\Documents\GitHub\GOS2022_root\Projects\Bootloader\GOS2022\GOS2022_bootloader" -L"C:\Users\Gabor\Documents\GitHub\GOS2022_root\Projects\Bootloader\GOS2022_lib\Lib_for_bootloader" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -u _printf_float -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

GOS2022_bootloader.list: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "GOS2022_bootloader.list"
	@echo 'Finished building: $@'
	@echo ' '

GOS2022_bootloader.bin: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-objcopy  -O binary $(EXECUTABLES) "GOS2022_bootloader.bin"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) GOS2022_bootloader.bin GOS2022_bootloader.elf GOS2022_bootloader.list GOS2022_bootloader.map default.size.stdout
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST) $(OBJCOPY_BIN)

fail-specified-linker-script-missing:
	@echo 'Error: Cannot find the specified linker script. Check the linker settings in the build configuration.'
	@exit 2

warn-no-linker-script-specified:
	@echo 'Warning: No linker script specified. Check the linker settings in the build configuration.'

.PHONY: all clean dependents main-build fail-specified-linker-script-missing warn-no-linker-script-specified
C:/Users/Gabor/Documents/GitHub/GOS2022_root/Projects/Bootloader/GOS2022_lib/Lib_for_bootloader/libGOS2022_lib.a:
C:/Users/Gabor/Documents/GitHub/GOS2022_root/Projects/Bootloader/GOS2022/GOS2022_bootloader/libGOS2022.a:

-include ../makefile.targets

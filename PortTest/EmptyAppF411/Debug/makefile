################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include Drivers/STM32F4xx_HAL_Driver/Src/subdir.mk
-include Driver/src/subdir.mk
-include Driver/cfg/subdir.mk
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


BUILD_ARTIFACT_NAME := EmptyAppF411
BUILD_ARTIFACT_EXTENSION := elf
BUILD_ARTIFACT_PREFIX :=
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME)$(if $(BUILD_ARTIFACT_EXTENSION),.$(BUILD_ARTIFACT_EXTENSION),)

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
EmptyAppF411.elf \

MAP_FILES += \
EmptyAppF411.map \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
EmptyAppF411.list \

OBJCOPY_BIN += \
EmptyAppF411.bin \


# All Target
all: main-build

# Main-build Target
main-build: EmptyAppF411.elf secondary-outputs

dependents:
	-cd C:/Users/Ahmed/Documents/GitHub/GOS2022/Projects/PortTest/GOS2022/Debug && $(MAKE) clean all
	-cd C:/Users/Ahmed/Documents/GitHub/GOS2022/Projects/PortTest/GOS2022_lib/Build_stm32f411xe && $(MAKE) clean all

# Tool invocations
EmptyAppF411.elf EmptyAppF411.map: $(OBJS) $(USER_OBJS) C:/Users/Ahmed/Documents/GitHub/GOS2022/Projects/PortTest/GOS2022/Debug/libGOS2022.a C:/Users/Ahmed/Documents/GitHub/GOS2022/Projects/PortTest/GOS2022_lib/Build_stm32f411xe/libGOS2022_lib.a C:\Users\Ahmed\Documents\GitHub\GOS2022\Projects\PortTest\EmptyAppF411\STM32F411RETX_FLASH.ld makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-gcc -o "EmptyAppF411.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m4 -T"C:\Users\Ahmed\Documents\GitHub\GOS2022\Projects\PortTest\EmptyAppF411\STM32F411RETX_FLASH.ld" --specs=nosys.specs -Wl,-Map="EmptyAppF411.map" -Wl,--gc-sections -static -L"C:\Users\Ahmed\Documents\GitHub\GOS2022\Projects\PortTest\GOS2022_lib\Build_stm32f411xe" -L"C:\Users\Ahmed\Documents\GitHub\GOS2022\Projects\PortTest\GOS2022\Debug" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

EmptyAppF411.list: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "EmptyAppF411.list"
	@echo 'Finished building: $@'
	@echo ' '

EmptyAppF411.bin: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-objcopy  -O binary $(EXECUTABLES) "EmptyAppF411.bin"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) EmptyAppF411.bin EmptyAppF411.elf EmptyAppF411.list EmptyAppF411.map default.size.stdout
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST) $(OBJCOPY_BIN)

fail-specified-linker-script-missing:
	@echo 'Error: Cannot find the specified linker script. Check the linker settings in the build configuration.'
	@exit 2

warn-no-linker-script-specified:
	@echo 'Warning: No linker script specified. Check the linker settings in the build configuration.'

.PHONY: all clean dependents main-build fail-specified-linker-script-missing warn-no-linker-script-specified
C:/Users/Ahmed/Documents/GitHub/GOS2022/Projects/PortTest/GOS2022/Debug/libGOS2022.a:
C:/Users/Ahmed/Documents/GitHub/GOS2022/Projects/PortTest/GOS2022_lib/Build_stm32f411xe/libGOS2022_lib.a:

-include ../makefile.targets

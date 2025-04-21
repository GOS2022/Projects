################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Src/gos_lib.c 

OBJS += \
./Src/gos_lib.o 

C_DEPS += \
./Src/gos_lib.d 


# Each subdirectory must supply rules for building sources it contributes
Src/%.o Src/%.su Src/%.cyclo: ../Src/%.c Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DSTM32 -DSTM32F4 -DSTM32F446RETx -DNUCLEO_F446RE -DSTM32F446xx -c -I"C:/Users/Gabor/Documents/GitHub/GOS2022_root/Projects/Bootloader/GOS2022" -I"C:/Users/Gabor/Documents/GitHub/GOS2022_root/Projects/Bootloader/GOS2022/os/kernel/inc" -I"C:/Users/Gabor/Documents/GitHub/GOS2022_root/Projects/Bootloader/GOS2022/os/driver/inc" -I"C:/Users/Gabor/Documents/GitHub/GOS2022_root/Projects/Bootloader/GOS2022/os/services/inc" -I"C:/Users/Gabor/Documents/GitHub/GOS2022_root/Projects/Bootloader/GOS2022_lib/drv/inc" -I"C:/Users/Gabor/Documents/GitHub/GOS2022_root/Projects/Bootloader/GOS2022_lib/bld/inc" -I"C:/Users/Gabor/Documents/GitHub/GOS2022_root/Projects/Bootloader/GOS2022_lib/Inc" -I"C:/Users/Gabor/Documents/GitHub/GOS2022_root/Projects/Bootloader/GOS2022_lib/hal/STM32F4xx_HAL_Driver/Inc" -I"C:/Users/Gabor/Documents/GitHub/GOS2022_root/Projects/Bootloader/GOS2022_lib/hal/STM32F4xx_HAL_Driver/Inc/Legacy" -I"C:/Users/Gabor/Documents/GitHub/GOS2022_root/Projects/Bootloader/GOS2022_lib/hal/CMSIS/Include" -I"C:/Users/Gabor/Documents/GitHub/GOS2022_root/Projects/Bootloader/GOS2022_lib/hal/CMSIS/Device/ST/STM32F4xx/Include" -I"C:/Users/Gabor/Documents/GitHub/GOS2022_root/Projects/Bootloader/GOS2022_lib/svl/inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Src

clean-Src:
	-$(RM) ./Src/gos_lib.cyclo ./Src/gos_lib.d ./Src/gos_lib.o ./Src/gos_lib.su

.PHONY: clean-Src


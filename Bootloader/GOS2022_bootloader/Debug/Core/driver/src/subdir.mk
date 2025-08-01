################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/driver/src/driver.c \
../Core/driver/src/drv_rtc.c \
../Core/driver/src/rcc_driver.c \
../Core/driver/src/trace_driver.c 

OBJS += \
./Core/driver/src/driver.o \
./Core/driver/src/drv_rtc.o \
./Core/driver/src/rcc_driver.o \
./Core/driver/src/trace_driver.o 

C_DEPS += \
./Core/driver/src/driver.d \
./Core/driver/src/drv_rtc.d \
./Core/driver/src/rcc_driver.d \
./Core/driver/src/trace_driver.d 


# Each subdirectory must supply rules for building sources it contributes
Core/driver/src/%.o Core/driver/src/%.su Core/driver/src/%.cyclo: ../Core/driver/src/%.c Core/driver/src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=c99 -g3 -DUSE_HAL_DRIVER -DSTM32F446xx -DDEBUG -c -I"C:/Users/Gabor/Documents/GitHub/GOS2022_root/Projects/Bootloader/GOS2022" -I"C:/Users/Gabor/Documents/GitHub/GOS2022_root/Projects/Bootloader/GOS2022_lib" -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"C:/Users/Gabor/Documents/GitHub/GOS2022_root/Projects/Bootloader/GOS2022/os/kernel/inc" -I"C:/Users/Gabor/Documents/GitHub/GOS2022_root/Projects/Bootloader/GOS2022/os/driver/inc" -I"C:/Users/Gabor/Documents/GitHub/GOS2022_root/Projects/Bootloader/GOS2022/os/services/inc" -I"C:/Users/Gabor/Documents/GitHub/GOS2022_root/Projects/Bootloader/GOS2022_lib/drv/inc" -I"C:/Users/Gabor/Documents/GitHub/GOS2022_root/Projects/Bootloader/GOS2022_lib/Inc" -I"C:/Users/Gabor/Documents/GitHub/GOS2022_root/Projects/Bootloader/GOS2022_lib/bld/inc" -I"C:/Users/Gabor/Documents/GitHub/GOS2022_root/Projects/Bootloader/GOS2022_bootloader/Core/driver/cfg" -I"C:/Users/Gabor/Documents/GitHub/GOS2022_root/Projects/Bootloader/GOS2022_lib/svl/inc" -I"C:/Users/Gabor/Documents/GitHub/GOS2022_root/Projects/Bootloader/GOS2022_bootloader/Core/driver/inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-driver-2f-src

clean-Core-2f-driver-2f-src:
	-$(RM) ./Core/driver/src/driver.cyclo ./Core/driver/src/driver.d ./Core/driver/src/driver.o ./Core/driver/src/driver.su ./Core/driver/src/drv_rtc.cyclo ./Core/driver/src/drv_rtc.d ./Core/driver/src/drv_rtc.o ./Core/driver/src/drv_rtc.su ./Core/driver/src/rcc_driver.cyclo ./Core/driver/src/rcc_driver.d ./Core/driver/src/rcc_driver.o ./Core/driver/src/rcc_driver.su ./Core/driver/src/trace_driver.cyclo ./Core/driver/src/trace_driver.d ./Core/driver/src/trace_driver.o ./Core/driver/src/trace_driver.su

.PHONY: clean-Core-2f-driver-2f-src


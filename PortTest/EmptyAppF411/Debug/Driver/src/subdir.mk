################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Driver/src/driver.c \
../Driver/src/rcc_driver.c 

OBJS += \
./Driver/src/driver.o \
./Driver/src/rcc_driver.o 

C_DEPS += \
./Driver/src/driver.d \
./Driver/src/rcc_driver.d 


# Each subdirectory must supply rules for building sources it contributes
Driver/src/%.o Driver/src/%.su Driver/src/%.cyclo: ../Driver/src/%.c Driver/src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F411xE -c -I"C:/Users/Ahmed/Documents/GitHub/GOS2022/Projects/PortTest/GOS2022_lib" -I"C:/Users/Ahmed/Documents/GitHub/GOS2022/Projects/PortTest/GOS2022" -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"C:/Users/Ahmed/Documents/GitHub/GOS2022/Projects/PortTest/GOS2022/os/driver/inc" -I"C:/Users/Ahmed/Documents/GitHub/GOS2022/Projects/PortTest/GOS2022/os/kernel/inc" -I"C:/Users/Ahmed/Documents/GitHub/GOS2022/Projects/PortTest/GOS2022/os/services/inc" -I"C:/Users/Ahmed/Documents/GitHub/GOS2022/Projects/PortTest/EmptyAppF411/Driver/cfg" -I"C:/Users/Ahmed/Documents/GitHub/GOS2022/Projects/PortTest/EmptyAppF411/Driver/inc" -I"C:/Users/Ahmed/Documents/GitHub/GOS2022/Projects/PortTest/GOS2022_lib/drv/inc" -I"C:/Users/Ahmed/Documents/GitHub/GOS2022/Projects/PortTest/GOS2022_lib/svl/inc" -I"C:/Users/Ahmed/Documents/GitHub/GOS2022/Projects/PortTest/GOS2022_lib/bld/inc" -I"C:/Users/Ahmed/Documents/GitHub/GOS2022/Projects/PortTest/GOS2022_lib/Inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Driver-2f-src

clean-Driver-2f-src:
	-$(RM) ./Driver/src/driver.cyclo ./Driver/src/driver.d ./Driver/src/driver.o ./Driver/src/driver.su ./Driver/src/rcc_driver.cyclo ./Driver/src/rcc_driver.d ./Driver/src/rcc_driver.o ./Driver/src/rcc_driver.su

.PHONY: clean-Driver-2f-src


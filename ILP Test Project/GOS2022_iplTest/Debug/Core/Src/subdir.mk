################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/app.c \
../Core/Src/app_iplBdSpec.c \
../Core/Src/app_led.c \
../Core/Src/syscalls.c \
../Core/Src/sysmem.c \
../Core/Src/system_stm32f4xx.c 

OBJS += \
./Core/Src/app.o \
./Core/Src/app_iplBdSpec.o \
./Core/Src/app_led.o \
./Core/Src/syscalls.o \
./Core/Src/sysmem.o \
./Core/Src/system_stm32f4xx.o 

C_DEPS += \
./Core/Src/app.d \
./Core/Src/app_iplBdSpec.d \
./Core/Src/app_led.d \
./Core/Src/syscalls.d \
./Core/Src/sysmem.d \
./Core/Src/system_stm32f4xx.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/%.o Core/Src/%.su Core/Src/%.cyclo: ../Core/Src/%.c Core/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=c99 -g3 -DUSE_HAL_DRIVER -DSTM32F446xx -DDEBUG -c -I"C:/Users/Gabor/STM32CubeIDE/workspace_1.5.1/GOS2022_lib" -I"C:/Users/Gabor/STM32CubeIDE/workspace_1.5.1/GOS2022" -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../FATFS/Target -I../FATFS/App -I../Middlewares/Third_Party/FatFs/src -I"C:/Users/Gabor/STM32CubeIDE/workspace_1.5.1/GOS2022/os/kernel/inc" -I"C:/Users/Gabor/STM32CubeIDE/workspace_1.5.1/GOS2022/os/driver/inc" -I"C:/Users/Gabor/STM32CubeIDE/workspace_1.5.1/GOS2022/os/services/inc" -I"C:/Users/Gabor/STM32CubeIDE/workspace_1.5.1/GOS2022_lib/drv/inc" -I"C:/Users/Gabor/STM32CubeIDE/workspace_1.5.1/GOS2022_lib/Inc" -I"C:/Users/Gabor/STM32CubeIDE/workspace_1.5.1/GOS2022_lib/bld/inc" -I"C:/Users/Gabor/STM32CubeIDE/workspace_1.5.1/GOS2022_iplTest/Core/driver/cfg" -I"C:/Users/Gabor/STM32CubeIDE/workspace_1.5.1/GOS2022_lib/svl/inc" -I"C:/Users/Gabor/STM32CubeIDE/workspace_1.5.1/GOS2022_iplTest/Core/gos_graphics/inc" -I"C:/Users/Gabor/STM32CubeIDE/workspace_1.5.1/GOS2022_iplTest/Core/driver/inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src

clean-Core-2f-Src:
	-$(RM) ./Core/Src/app.cyclo ./Core/Src/app.d ./Core/Src/app.o ./Core/Src/app.su ./Core/Src/app_iplBdSpec.cyclo ./Core/Src/app_iplBdSpec.d ./Core/Src/app_iplBdSpec.o ./Core/Src/app_iplBdSpec.su ./Core/Src/app_led.cyclo ./Core/Src/app_led.d ./Core/Src/app_led.o ./Core/Src/app_led.su ./Core/Src/syscalls.cyclo ./Core/Src/syscalls.d ./Core/Src/syscalls.o ./Core/Src/syscalls.su ./Core/Src/sysmem.cyclo ./Core/Src/sysmem.d ./Core/Src/sysmem.o ./Core/Src/sysmem.su ./Core/Src/system_stm32f4xx.cyclo ./Core/Src/system_stm32f4xx.d ./Core/Src/system_stm32f4xx.o ./Core/Src/system_stm32f4xx.su

.PHONY: clean-Core-2f-Src


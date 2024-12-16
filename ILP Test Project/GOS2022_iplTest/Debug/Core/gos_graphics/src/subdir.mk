################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/gos_graphics/src/g_button.c \
../Core/gos_graphics/src/g_control.c \
../Core/gos_graphics/src/g_driver.c \
../Core/gos_graphics/src/g_label.c \
../Core/gos_graphics/src/g_window.c 

OBJS += \
./Core/gos_graphics/src/g_button.o \
./Core/gos_graphics/src/g_control.o \
./Core/gos_graphics/src/g_driver.o \
./Core/gos_graphics/src/g_label.o \
./Core/gos_graphics/src/g_window.o 

C_DEPS += \
./Core/gos_graphics/src/g_button.d \
./Core/gos_graphics/src/g_control.d \
./Core/gos_graphics/src/g_driver.d \
./Core/gos_graphics/src/g_label.d \
./Core/gos_graphics/src/g_window.d 


# Each subdirectory must supply rules for building sources it contributes
Core/gos_graphics/src/%.o Core/gos_graphics/src/%.su Core/gos_graphics/src/%.cyclo: ../Core/gos_graphics/src/%.c Core/gos_graphics/src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=c99 -g3 -DUSE_HAL_DRIVER -DSTM32F446xx -DDEBUG -c -I"C:/Users/Gabor/STM32CubeIDE/workspace_1.5.1/GOS2022_lib" -I"C:/Users/Gabor/STM32CubeIDE/workspace_1.5.1/GOS2022" -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../FATFS/Target -I../FATFS/App -I../Middlewares/Third_Party/FatFs/src -I"C:/Users/Gabor/STM32CubeIDE/workspace_1.5.1/GOS2022/os/kernel/inc" -I"C:/Users/Gabor/STM32CubeIDE/workspace_1.5.1/GOS2022/os/driver/inc" -I"C:/Users/Gabor/STM32CubeIDE/workspace_1.5.1/GOS2022/os/services/inc" -I"C:/Users/Gabor/STM32CubeIDE/workspace_1.5.1/GOS2022_lib/drv/inc" -I"C:/Users/Gabor/STM32CubeIDE/workspace_1.5.1/GOS2022_lib/Inc" -I"C:/Users/Gabor/STM32CubeIDE/workspace_1.5.1/GOS2022_lib/bld/inc" -I"C:/Users/Gabor/STM32CubeIDE/workspace_1.5.1/GOS2022_iplTest/Core/driver/cfg" -I"C:/Users/Gabor/STM32CubeIDE/workspace_1.5.1/GOS2022_lib/svl/inc" -I"C:/Users/Gabor/STM32CubeIDE/workspace_1.5.1/GOS2022_iplTest/Core/gos_graphics/inc" -I"C:/Users/Gabor/STM32CubeIDE/workspace_1.5.1/GOS2022_iplTest/Core/driver/inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-gos_graphics-2f-src

clean-Core-2f-gos_graphics-2f-src:
	-$(RM) ./Core/gos_graphics/src/g_button.cyclo ./Core/gos_graphics/src/g_button.d ./Core/gos_graphics/src/g_button.o ./Core/gos_graphics/src/g_button.su ./Core/gos_graphics/src/g_control.cyclo ./Core/gos_graphics/src/g_control.d ./Core/gos_graphics/src/g_control.o ./Core/gos_graphics/src/g_control.su ./Core/gos_graphics/src/g_driver.cyclo ./Core/gos_graphics/src/g_driver.d ./Core/gos_graphics/src/g_driver.o ./Core/gos_graphics/src/g_driver.su ./Core/gos_graphics/src/g_label.cyclo ./Core/gos_graphics/src/g_label.d ./Core/gos_graphics/src/g_label.o ./Core/gos_graphics/src/g_label.su ./Core/gos_graphics/src/g_window.cyclo ./Core/gos_graphics/src/g_window.d ./Core/gos_graphics/src/g_window.o ./Core/gos_graphics/src/g_window.su

.PHONY: clean-Core-2f-gos_graphics-2f-src


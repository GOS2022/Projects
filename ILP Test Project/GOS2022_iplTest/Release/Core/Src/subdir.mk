################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/app.c \
../Core/Src/app_button.c \
../Core/Src/app_counter.c \
../Core/Src/app_dac.c \
../Core/Src/app_deadlock.c \
../Core/Src/app_debouncer.c \
../Core/Src/app_gcpTest.c \
../Core/Src/app_led.c \
../Core/Src/app_menu.c \
../Core/Src/app_message.c \
../Core/Src/app_priorityInversion.c \
../Core/Src/app_pwmled.c \
../Core/Src/app_queue.c \
../Core/Src/app_sd.c \
../Core/Src/app_shell.c \
../Core/Src/app_singleshot.c \
../Core/Src/app_tft.c \
../Core/Src/app_time.c \
../Core/Src/app_timed.c \
../Core/Src/syscalls.c \
../Core/Src/sysmem.c \
../Core/Src/system_stm32f4xx.c 

OBJS += \
./Core/Src/app.o \
./Core/Src/app_button.o \
./Core/Src/app_counter.o \
./Core/Src/app_dac.o \
./Core/Src/app_deadlock.o \
./Core/Src/app_debouncer.o \
./Core/Src/app_gcpTest.o \
./Core/Src/app_led.o \
./Core/Src/app_menu.o \
./Core/Src/app_message.o \
./Core/Src/app_priorityInversion.o \
./Core/Src/app_pwmled.o \
./Core/Src/app_queue.o \
./Core/Src/app_sd.o \
./Core/Src/app_shell.o \
./Core/Src/app_singleshot.o \
./Core/Src/app_tft.o \
./Core/Src/app_time.o \
./Core/Src/app_timed.o \
./Core/Src/syscalls.o \
./Core/Src/sysmem.o \
./Core/Src/system_stm32f4xx.o 

C_DEPS += \
./Core/Src/app.d \
./Core/Src/app_button.d \
./Core/Src/app_counter.d \
./Core/Src/app_dac.d \
./Core/Src/app_deadlock.d \
./Core/Src/app_debouncer.d \
./Core/Src/app_gcpTest.d \
./Core/Src/app_led.d \
./Core/Src/app_menu.d \
./Core/Src/app_message.d \
./Core/Src/app_priorityInversion.d \
./Core/Src/app_pwmled.d \
./Core/Src/app_queue.d \
./Core/Src/app_sd.d \
./Core/Src/app_shell.d \
./Core/Src/app_singleshot.d \
./Core/Src/app_tft.d \
./Core/Src/app_time.d \
./Core/Src/app_timed.d \
./Core/Src/syscalls.d \
./Core/Src/sysmem.d \
./Core/Src/system_stm32f4xx.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/%.o Core/Src/%.su Core/Src/%.cyclo: ../Core/Src/%.c Core/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -DUSE_HAL_DRIVER -DSTM32F446xx -c -I"C:/Users/Gabor/STM32CubeIDE/workspace_1.5.1/GOS2022/os/driver/inc" -I"C:/Users/Gabor/STM32CubeIDE/workspace_1.5.1/GOS2022/os/kernel/inc" -I"C:/Users/Gabor/STM32CubeIDE/workspace_1.5.1/GOS2022/os/services/inc" -I"C:/Users/Gabor/STM32CubeIDE/workspace_1.5.1/GOS2022_test2/Core/driver/inc" -I"C:/Users/Gabor/STM32CubeIDE/workspace_1.5.1/GOS2022_test2/Core/gos_graphics/inc" -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../FATFS/Target -I../FATFS/App -I../Middlewares/Third_Party/FatFs/src -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src

clean-Core-2f-Src:
	-$(RM) ./Core/Src/app.cyclo ./Core/Src/app.d ./Core/Src/app.o ./Core/Src/app.su ./Core/Src/app_button.cyclo ./Core/Src/app_button.d ./Core/Src/app_button.o ./Core/Src/app_button.su ./Core/Src/app_counter.cyclo ./Core/Src/app_counter.d ./Core/Src/app_counter.o ./Core/Src/app_counter.su ./Core/Src/app_dac.cyclo ./Core/Src/app_dac.d ./Core/Src/app_dac.o ./Core/Src/app_dac.su ./Core/Src/app_deadlock.cyclo ./Core/Src/app_deadlock.d ./Core/Src/app_deadlock.o ./Core/Src/app_deadlock.su ./Core/Src/app_debouncer.cyclo ./Core/Src/app_debouncer.d ./Core/Src/app_debouncer.o ./Core/Src/app_debouncer.su ./Core/Src/app_gcpTest.cyclo ./Core/Src/app_gcpTest.d ./Core/Src/app_gcpTest.o ./Core/Src/app_gcpTest.su ./Core/Src/app_led.cyclo ./Core/Src/app_led.d ./Core/Src/app_led.o ./Core/Src/app_led.su ./Core/Src/app_menu.cyclo ./Core/Src/app_menu.d ./Core/Src/app_menu.o ./Core/Src/app_menu.su ./Core/Src/app_message.cyclo ./Core/Src/app_message.d ./Core/Src/app_message.o ./Core/Src/app_message.su ./Core/Src/app_priorityInversion.cyclo ./Core/Src/app_priorityInversion.d ./Core/Src/app_priorityInversion.o ./Core/Src/app_priorityInversion.su ./Core/Src/app_pwmled.cyclo ./Core/Src/app_pwmled.d ./Core/Src/app_pwmled.o ./Core/Src/app_pwmled.su ./Core/Src/app_queue.cyclo ./Core/Src/app_queue.d ./Core/Src/app_queue.o ./Core/Src/app_queue.su ./Core/Src/app_sd.cyclo ./Core/Src/app_sd.d ./Core/Src/app_sd.o ./Core/Src/app_sd.su ./Core/Src/app_shell.cyclo ./Core/Src/app_shell.d ./Core/Src/app_shell.o ./Core/Src/app_shell.su ./Core/Src/app_singleshot.cyclo ./Core/Src/app_singleshot.d ./Core/Src/app_singleshot.o ./Core/Src/app_singleshot.su ./Core/Src/app_tft.cyclo ./Core/Src/app_tft.d ./Core/Src/app_tft.o ./Core/Src/app_tft.su ./Core/Src/app_time.cyclo ./Core/Src/app_time.d ./Core/Src/app_time.o ./Core/Src/app_time.su ./Core/Src/app_timed.cyclo ./Core/Src/app_timed.d ./Core/Src/app_timed.o ./Core/Src/app_timed.su ./Core/Src/syscalls.cyclo ./Core/Src/syscalls.d ./Core/Src/syscalls.o ./Core/Src/syscalls.su ./Core/Src/sysmem.cyclo ./Core/Src/sysmem.d ./Core/Src/sysmem.o ./Core/Src/sysmem.su ./Core/Src/system_stm32f4xx.cyclo ./Core/Src/system_stm32f4xx.d ./Core/Src/system_stm32f4xx.o ./Core/Src/system_stm32f4xx.su

.PHONY: clean-Core-2f-Src


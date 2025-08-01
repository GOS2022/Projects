################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../drv/src/drv_25lc256.c \
../drv/src/drv_25lc640.c \
../drv/src/drv_adc.c \
../drv/src/drv_crc.c \
../drv/src/drv_dac.c \
../drv/src/drv_dma.c \
../drv/src/drv_ds3231.c \
../drv/src/drv_error.c \
../drv/src/drv_flash.c \
../drv/src/drv_gpio.c \
../drv/src/drv_i2c.c \
../drv/src/drv_it.c \
../drv/src/drv_lcd.c \
../drv/src/drv_mcp23017.c \
../drv/src/drv_shell.c \
../drv/src/drv_spi.c \
../drv/src/drv_sysmon.c \
../drv/src/drv_systimer.c \
../drv/src/drv_tmr.c \
../drv/src/drv_trace.c \
../drv/src/drv_uart.c \
../drv/src/drv_w25q64.c 

OBJS += \
./drv/src/drv_25lc256.o \
./drv/src/drv_25lc640.o \
./drv/src/drv_adc.o \
./drv/src/drv_crc.o \
./drv/src/drv_dac.o \
./drv/src/drv_dma.o \
./drv/src/drv_ds3231.o \
./drv/src/drv_error.o \
./drv/src/drv_flash.o \
./drv/src/drv_gpio.o \
./drv/src/drv_i2c.o \
./drv/src/drv_it.o \
./drv/src/drv_lcd.o \
./drv/src/drv_mcp23017.o \
./drv/src/drv_shell.o \
./drv/src/drv_spi.o \
./drv/src/drv_sysmon.o \
./drv/src/drv_systimer.o \
./drv/src/drv_tmr.o \
./drv/src/drv_trace.o \
./drv/src/drv_uart.o \
./drv/src/drv_w25q64.o 

C_DEPS += \
./drv/src/drv_25lc256.d \
./drv/src/drv_25lc640.d \
./drv/src/drv_adc.d \
./drv/src/drv_crc.d \
./drv/src/drv_dac.d \
./drv/src/drv_dma.d \
./drv/src/drv_ds3231.d \
./drv/src/drv_error.d \
./drv/src/drv_flash.d \
./drv/src/drv_gpio.d \
./drv/src/drv_i2c.d \
./drv/src/drv_it.d \
./drv/src/drv_lcd.d \
./drv/src/drv_mcp23017.d \
./drv/src/drv_shell.d \
./drv/src/drv_spi.d \
./drv/src/drv_sysmon.d \
./drv/src/drv_systimer.d \
./drv/src/drv_tmr.d \
./drv/src/drv_trace.d \
./drv/src/drv_uart.d \
./drv/src/drv_w25q64.d 


# Each subdirectory must supply rules for building sources it contributes
drv/src/%.o drv/src/%.su drv/src/%.cyclo: ../drv/src/%.c drv/src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DSTM32 -DSTM32F4 -DSTM32F446RETx -DNUCLEO_F446RE -DSTM32F446xx -c -I"C:/Users/Gabor/Documents/GitHub/GOS2022_root/Projects/Bootloader/GOS2022" -I"C:/Users/Gabor/Documents/GitHub/GOS2022_root/Projects/Bootloader/GOS2022/os/kernel/inc" -I"C:/Users/Gabor/Documents/GitHub/GOS2022_root/Projects/Bootloader/GOS2022/os/driver/inc" -I"C:/Users/Gabor/Documents/GitHub/GOS2022_root/Projects/Bootloader/GOS2022/os/services/inc" -I"C:/Users/Gabor/Documents/GitHub/GOS2022_root/Projects/Bootloader/GOS2022_lib/drv/inc" -I"C:/Users/Gabor/Documents/GitHub/GOS2022_root/Projects/Bootloader/GOS2022_lib/bld/inc" -I"C:/Users/Gabor/Documents/GitHub/GOS2022_root/Projects/Bootloader/GOS2022_lib/Inc" -I"C:/Users/Gabor/Documents/GitHub/GOS2022_root/Projects/Bootloader/GOS2022_lib/hal/STM32F4xx_HAL_Driver/Inc" -I"C:/Users/Gabor/Documents/GitHub/GOS2022_root/Projects/Bootloader/GOS2022_lib/hal/STM32F4xx_HAL_Driver/Inc/Legacy" -I"C:/Users/Gabor/Documents/GitHub/GOS2022_root/Projects/Bootloader/GOS2022_lib/hal/CMSIS/Include" -I"C:/Users/Gabor/Documents/GitHub/GOS2022_root/Projects/Bootloader/GOS2022_lib/hal/CMSIS/Device/ST/STM32F4xx/Include" -I"C:/Users/Gabor/Documents/GitHub/GOS2022_root/Projects/Bootloader/GOS2022_lib/svl/inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-drv-2f-src

clean-drv-2f-src:
	-$(RM) ./drv/src/drv_25lc256.cyclo ./drv/src/drv_25lc256.d ./drv/src/drv_25lc256.o ./drv/src/drv_25lc256.su ./drv/src/drv_25lc640.cyclo ./drv/src/drv_25lc640.d ./drv/src/drv_25lc640.o ./drv/src/drv_25lc640.su ./drv/src/drv_adc.cyclo ./drv/src/drv_adc.d ./drv/src/drv_adc.o ./drv/src/drv_adc.su ./drv/src/drv_crc.cyclo ./drv/src/drv_crc.d ./drv/src/drv_crc.o ./drv/src/drv_crc.su ./drv/src/drv_dac.cyclo ./drv/src/drv_dac.d ./drv/src/drv_dac.o ./drv/src/drv_dac.su ./drv/src/drv_dma.cyclo ./drv/src/drv_dma.d ./drv/src/drv_dma.o ./drv/src/drv_dma.su ./drv/src/drv_ds3231.cyclo ./drv/src/drv_ds3231.d ./drv/src/drv_ds3231.o ./drv/src/drv_ds3231.su ./drv/src/drv_error.cyclo ./drv/src/drv_error.d ./drv/src/drv_error.o ./drv/src/drv_error.su ./drv/src/drv_flash.cyclo ./drv/src/drv_flash.d ./drv/src/drv_flash.o ./drv/src/drv_flash.su ./drv/src/drv_gpio.cyclo ./drv/src/drv_gpio.d ./drv/src/drv_gpio.o ./drv/src/drv_gpio.su ./drv/src/drv_i2c.cyclo ./drv/src/drv_i2c.d ./drv/src/drv_i2c.o ./drv/src/drv_i2c.su ./drv/src/drv_it.cyclo ./drv/src/drv_it.d ./drv/src/drv_it.o ./drv/src/drv_it.su ./drv/src/drv_lcd.cyclo ./drv/src/drv_lcd.d ./drv/src/drv_lcd.o ./drv/src/drv_lcd.su ./drv/src/drv_mcp23017.cyclo ./drv/src/drv_mcp23017.d ./drv/src/drv_mcp23017.o ./drv/src/drv_mcp23017.su ./drv/src/drv_shell.cyclo ./drv/src/drv_shell.d ./drv/src/drv_shell.o ./drv/src/drv_shell.su ./drv/src/drv_spi.cyclo ./drv/src/drv_spi.d ./drv/src/drv_spi.o ./drv/src/drv_spi.su ./drv/src/drv_sysmon.cyclo ./drv/src/drv_sysmon.d ./drv/src/drv_sysmon.o ./drv/src/drv_sysmon.su ./drv/src/drv_systimer.cyclo ./drv/src/drv_systimer.d ./drv/src/drv_systimer.o ./drv/src/drv_systimer.su ./drv/src/drv_tmr.cyclo ./drv/src/drv_tmr.d ./drv/src/drv_tmr.o ./drv/src/drv_tmr.su ./drv/src/drv_trace.cyclo ./drv/src/drv_trace.d ./drv/src/drv_trace.o ./drv/src/drv_trace.su ./drv/src/drv_uart.cyclo ./drv/src/drv_uart.d ./drv/src/drv_uart.o ./drv/src/drv_uart.su ./drv/src/drv_w25q64.cyclo ./drv/src/drv_w25q64.d ./drv/src/drv_w25q64.o ./drv/src/drv_w25q64.su

.PHONY: clean-drv-2f-src


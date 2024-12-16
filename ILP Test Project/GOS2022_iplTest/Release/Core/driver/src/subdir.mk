################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/driver/src/adc_driver.c \
../Core/driver/src/dac_driver.c \
../Core/driver/src/dma_driver.c \
../Core/driver/src/eeprom_driver.c \
../Core/driver/src/file_driver.c \
../Core/driver/src/gpio_driver.c \
../Core/driver/src/i2c_driver.c \
../Core/driver/src/ili9341.c \
../Core/driver/src/ili9341_font.c \
../Core/driver/src/ili9341_gfx.c \
../Core/driver/src/lcd_driver.c \
../Core/driver/src/pwm_driver.c \
../Core/driver/src/rcc_driver.c \
../Core/driver/src/rtc_driver.c \
../Core/driver/src/sd_driver.c \
../Core/driver/src/spi_driver.c \
../Core/driver/src/temperature_driver.c \
../Core/driver/src/timer_driver.c \
../Core/driver/src/uart_driver.c \
../Core/driver/src/z_displ_ILI9XXX.c \
../Core/driver/src/z_touch_XPT2046.c 

OBJS += \
./Core/driver/src/adc_driver.o \
./Core/driver/src/dac_driver.o \
./Core/driver/src/dma_driver.o \
./Core/driver/src/eeprom_driver.o \
./Core/driver/src/file_driver.o \
./Core/driver/src/gpio_driver.o \
./Core/driver/src/i2c_driver.o \
./Core/driver/src/ili9341.o \
./Core/driver/src/ili9341_font.o \
./Core/driver/src/ili9341_gfx.o \
./Core/driver/src/lcd_driver.o \
./Core/driver/src/pwm_driver.o \
./Core/driver/src/rcc_driver.o \
./Core/driver/src/rtc_driver.o \
./Core/driver/src/sd_driver.o \
./Core/driver/src/spi_driver.o \
./Core/driver/src/temperature_driver.o \
./Core/driver/src/timer_driver.o \
./Core/driver/src/uart_driver.o \
./Core/driver/src/z_displ_ILI9XXX.o \
./Core/driver/src/z_touch_XPT2046.o 

C_DEPS += \
./Core/driver/src/adc_driver.d \
./Core/driver/src/dac_driver.d \
./Core/driver/src/dma_driver.d \
./Core/driver/src/eeprom_driver.d \
./Core/driver/src/file_driver.d \
./Core/driver/src/gpio_driver.d \
./Core/driver/src/i2c_driver.d \
./Core/driver/src/ili9341.d \
./Core/driver/src/ili9341_font.d \
./Core/driver/src/ili9341_gfx.d \
./Core/driver/src/lcd_driver.d \
./Core/driver/src/pwm_driver.d \
./Core/driver/src/rcc_driver.d \
./Core/driver/src/rtc_driver.d \
./Core/driver/src/sd_driver.d \
./Core/driver/src/spi_driver.d \
./Core/driver/src/temperature_driver.d \
./Core/driver/src/timer_driver.d \
./Core/driver/src/uart_driver.d \
./Core/driver/src/z_displ_ILI9XXX.d \
./Core/driver/src/z_touch_XPT2046.d 


# Each subdirectory must supply rules for building sources it contributes
Core/driver/src/%.o Core/driver/src/%.su Core/driver/src/%.cyclo: ../Core/driver/src/%.c Core/driver/src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -DUSE_HAL_DRIVER -DSTM32F446xx -c -I"C:/Users/Gabor/STM32CubeIDE/workspace_1.5.1/GOS2022/os/driver/inc" -I"C:/Users/Gabor/STM32CubeIDE/workspace_1.5.1/GOS2022/os/kernel/inc" -I"C:/Users/Gabor/STM32CubeIDE/workspace_1.5.1/GOS2022/os/services/inc" -I"C:/Users/Gabor/STM32CubeIDE/workspace_1.5.1/GOS2022_test2/Core/driver/inc" -I"C:/Users/Gabor/STM32CubeIDE/workspace_1.5.1/GOS2022_test2/Core/gos_graphics/inc" -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../FATFS/Target -I../FATFS/App -I../Middlewares/Third_Party/FatFs/src -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-driver-2f-src

clean-Core-2f-driver-2f-src:
	-$(RM) ./Core/driver/src/adc_driver.cyclo ./Core/driver/src/adc_driver.d ./Core/driver/src/adc_driver.o ./Core/driver/src/adc_driver.su ./Core/driver/src/dac_driver.cyclo ./Core/driver/src/dac_driver.d ./Core/driver/src/dac_driver.o ./Core/driver/src/dac_driver.su ./Core/driver/src/dma_driver.cyclo ./Core/driver/src/dma_driver.d ./Core/driver/src/dma_driver.o ./Core/driver/src/dma_driver.su ./Core/driver/src/eeprom_driver.cyclo ./Core/driver/src/eeprom_driver.d ./Core/driver/src/eeprom_driver.o ./Core/driver/src/eeprom_driver.su ./Core/driver/src/file_driver.cyclo ./Core/driver/src/file_driver.d ./Core/driver/src/file_driver.o ./Core/driver/src/file_driver.su ./Core/driver/src/gpio_driver.cyclo ./Core/driver/src/gpio_driver.d ./Core/driver/src/gpio_driver.o ./Core/driver/src/gpio_driver.su ./Core/driver/src/i2c_driver.cyclo ./Core/driver/src/i2c_driver.d ./Core/driver/src/i2c_driver.o ./Core/driver/src/i2c_driver.su ./Core/driver/src/ili9341.cyclo ./Core/driver/src/ili9341.d ./Core/driver/src/ili9341.o ./Core/driver/src/ili9341.su ./Core/driver/src/ili9341_font.cyclo ./Core/driver/src/ili9341_font.d ./Core/driver/src/ili9341_font.o ./Core/driver/src/ili9341_font.su ./Core/driver/src/ili9341_gfx.cyclo ./Core/driver/src/ili9341_gfx.d ./Core/driver/src/ili9341_gfx.o ./Core/driver/src/ili9341_gfx.su ./Core/driver/src/lcd_driver.cyclo ./Core/driver/src/lcd_driver.d ./Core/driver/src/lcd_driver.o ./Core/driver/src/lcd_driver.su ./Core/driver/src/pwm_driver.cyclo ./Core/driver/src/pwm_driver.d ./Core/driver/src/pwm_driver.o ./Core/driver/src/pwm_driver.su ./Core/driver/src/rcc_driver.cyclo ./Core/driver/src/rcc_driver.d ./Core/driver/src/rcc_driver.o ./Core/driver/src/rcc_driver.su ./Core/driver/src/rtc_driver.cyclo ./Core/driver/src/rtc_driver.d ./Core/driver/src/rtc_driver.o ./Core/driver/src/rtc_driver.su ./Core/driver/src/sd_driver.cyclo ./Core/driver/src/sd_driver.d ./Core/driver/src/sd_driver.o ./Core/driver/src/sd_driver.su ./Core/driver/src/spi_driver.cyclo ./Core/driver/src/spi_driver.d ./Core/driver/src/spi_driver.o ./Core/driver/src/spi_driver.su ./Core/driver/src/temperature_driver.cyclo ./Core/driver/src/temperature_driver.d ./Core/driver/src/temperature_driver.o ./Core/driver/src/temperature_driver.su ./Core/driver/src/timer_driver.cyclo ./Core/driver/src/timer_driver.d ./Core/driver/src/timer_driver.o ./Core/driver/src/timer_driver.su ./Core/driver/src/uart_driver.cyclo ./Core/driver/src/uart_driver.d ./Core/driver/src/uart_driver.o ./Core/driver/src/uart_driver.su ./Core/driver/src/z_displ_ILI9XXX.cyclo ./Core/driver/src/z_displ_ILI9XXX.d ./Core/driver/src/z_displ_ILI9XXX.o ./Core/driver/src/z_displ_ILI9XXX.su ./Core/driver/src/z_touch_XPT2046.cyclo ./Core/driver/src/z_touch_XPT2046.d ./Core/driver/src/z_touch_XPT2046.o ./Core/driver/src/z_touch_XPT2046.su

.PHONY: clean-Core-2f-driver-2f-src


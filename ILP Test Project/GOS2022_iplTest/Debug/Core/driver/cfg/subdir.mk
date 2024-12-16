################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/driver/cfg/adc_cfg.c \
../Core/driver/cfg/dac_cfg.c \
../Core/driver/cfg/dma_cfg.c \
../Core/driver/cfg/dsm_cfg.c \
../Core/driver/cfg/gpio_cfg.c \
../Core/driver/cfg/i2c_cfg.c \
../Core/driver/cfg/it_cfg.c \
../Core/driver/cfg/spi_cfg.c \
../Core/driver/cfg/tmr_cfg.c \
../Core/driver/cfg/uart_cfg.c 

OBJS += \
./Core/driver/cfg/adc_cfg.o \
./Core/driver/cfg/dac_cfg.o \
./Core/driver/cfg/dma_cfg.o \
./Core/driver/cfg/dsm_cfg.o \
./Core/driver/cfg/gpio_cfg.o \
./Core/driver/cfg/i2c_cfg.o \
./Core/driver/cfg/it_cfg.o \
./Core/driver/cfg/spi_cfg.o \
./Core/driver/cfg/tmr_cfg.o \
./Core/driver/cfg/uart_cfg.o 

C_DEPS += \
./Core/driver/cfg/adc_cfg.d \
./Core/driver/cfg/dac_cfg.d \
./Core/driver/cfg/dma_cfg.d \
./Core/driver/cfg/dsm_cfg.d \
./Core/driver/cfg/gpio_cfg.d \
./Core/driver/cfg/i2c_cfg.d \
./Core/driver/cfg/it_cfg.d \
./Core/driver/cfg/spi_cfg.d \
./Core/driver/cfg/tmr_cfg.d \
./Core/driver/cfg/uart_cfg.d 


# Each subdirectory must supply rules for building sources it contributes
Core/driver/cfg/%.o Core/driver/cfg/%.su Core/driver/cfg/%.cyclo: ../Core/driver/cfg/%.c Core/driver/cfg/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=c99 -g3 -DUSE_HAL_DRIVER -DSTM32F446xx -DDEBUG -c -I"C:/Users/Gabor/STM32CubeIDE/workspace_1.5.1/GOS2022_lib" -I"C:/Users/Gabor/STM32CubeIDE/workspace_1.5.1/GOS2022" -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../FATFS/Target -I../FATFS/App -I../Middlewares/Third_Party/FatFs/src -I"C:/Users/Gabor/STM32CubeIDE/workspace_1.5.1/GOS2022/os/kernel/inc" -I"C:/Users/Gabor/STM32CubeIDE/workspace_1.5.1/GOS2022/os/driver/inc" -I"C:/Users/Gabor/STM32CubeIDE/workspace_1.5.1/GOS2022/os/services/inc" -I"C:/Users/Gabor/STM32CubeIDE/workspace_1.5.1/GOS2022_lib/drv/inc" -I"C:/Users/Gabor/STM32CubeIDE/workspace_1.5.1/GOS2022_lib/Inc" -I"C:/Users/Gabor/STM32CubeIDE/workspace_1.5.1/GOS2022_lib/bld/inc" -I"C:/Users/Gabor/STM32CubeIDE/workspace_1.5.1/GOS2022_iplTest/Core/driver/cfg" -I"C:/Users/Gabor/STM32CubeIDE/workspace_1.5.1/GOS2022_lib/svl/inc" -I"C:/Users/Gabor/STM32CubeIDE/workspace_1.5.1/GOS2022_iplTest/Core/gos_graphics/inc" -I"C:/Users/Gabor/STM32CubeIDE/workspace_1.5.1/GOS2022_iplTest/Core/driver/inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-driver-2f-cfg

clean-Core-2f-driver-2f-cfg:
	-$(RM) ./Core/driver/cfg/adc_cfg.cyclo ./Core/driver/cfg/adc_cfg.d ./Core/driver/cfg/adc_cfg.o ./Core/driver/cfg/adc_cfg.su ./Core/driver/cfg/dac_cfg.cyclo ./Core/driver/cfg/dac_cfg.d ./Core/driver/cfg/dac_cfg.o ./Core/driver/cfg/dac_cfg.su ./Core/driver/cfg/dma_cfg.cyclo ./Core/driver/cfg/dma_cfg.d ./Core/driver/cfg/dma_cfg.o ./Core/driver/cfg/dma_cfg.su ./Core/driver/cfg/dsm_cfg.cyclo ./Core/driver/cfg/dsm_cfg.d ./Core/driver/cfg/dsm_cfg.o ./Core/driver/cfg/dsm_cfg.su ./Core/driver/cfg/gpio_cfg.cyclo ./Core/driver/cfg/gpio_cfg.d ./Core/driver/cfg/gpio_cfg.o ./Core/driver/cfg/gpio_cfg.su ./Core/driver/cfg/i2c_cfg.cyclo ./Core/driver/cfg/i2c_cfg.d ./Core/driver/cfg/i2c_cfg.o ./Core/driver/cfg/i2c_cfg.su ./Core/driver/cfg/it_cfg.cyclo ./Core/driver/cfg/it_cfg.d ./Core/driver/cfg/it_cfg.o ./Core/driver/cfg/it_cfg.su ./Core/driver/cfg/spi_cfg.cyclo ./Core/driver/cfg/spi_cfg.d ./Core/driver/cfg/spi_cfg.o ./Core/driver/cfg/spi_cfg.su ./Core/driver/cfg/tmr_cfg.cyclo ./Core/driver/cfg/tmr_cfg.d ./Core/driver/cfg/tmr_cfg.o ./Core/driver/cfg/tmr_cfg.su ./Core/driver/cfg/uart_cfg.cyclo ./Core/driver/cfg/uart_cfg.d ./Core/driver/cfg/uart_cfg.o ./Core/driver/cfg/uart_cfg.su

.PHONY: clean-Core-2f-driver-2f-cfg

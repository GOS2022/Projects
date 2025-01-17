################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Driver/cfg/dma_cfg.c \
../Driver/cfg/dsm_cfg.c \
../Driver/cfg/gpio_cfg.c \
../Driver/cfg/i2c_cfg.c \
../Driver/cfg/it_cfg.c \
../Driver/cfg/spi_cfg.c \
../Driver/cfg/tmr_cfg.c \
../Driver/cfg/uart_cfg.c 

OBJS += \
./Driver/cfg/dma_cfg.o \
./Driver/cfg/dsm_cfg.o \
./Driver/cfg/gpio_cfg.o \
./Driver/cfg/i2c_cfg.o \
./Driver/cfg/it_cfg.o \
./Driver/cfg/spi_cfg.o \
./Driver/cfg/tmr_cfg.o \
./Driver/cfg/uart_cfg.o 

C_DEPS += \
./Driver/cfg/dma_cfg.d \
./Driver/cfg/dsm_cfg.d \
./Driver/cfg/gpio_cfg.d \
./Driver/cfg/i2c_cfg.d \
./Driver/cfg/it_cfg.d \
./Driver/cfg/spi_cfg.d \
./Driver/cfg/tmr_cfg.d \
./Driver/cfg/uart_cfg.d 


# Each subdirectory must supply rules for building sources it contributes
Driver/cfg/%.o Driver/cfg/%.su Driver/cfg/%.cyclo: ../Driver/cfg/%.c Driver/cfg/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F411xE -c -I"C:/Users/Ahmed/Documents/GitHub/GOS2022/Projects/PortTest/GOS2022_lib" -I"C:/Users/Ahmed/Documents/GitHub/GOS2022/Projects/PortTest/GOS2022" -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"C:/Users/Ahmed/Documents/GitHub/GOS2022/Projects/PortTest/GOS2022/os/driver/inc" -I"C:/Users/Ahmed/Documents/GitHub/GOS2022/Projects/PortTest/GOS2022/os/kernel/inc" -I"C:/Users/Ahmed/Documents/GitHub/GOS2022/Projects/PortTest/GOS2022/os/services/inc" -I"C:/Users/Ahmed/Documents/GitHub/GOS2022/Projects/PortTest/EmptyAppF411/Driver/cfg" -I"C:/Users/Ahmed/Documents/GitHub/GOS2022/Projects/PortTest/EmptyAppF411/Driver/inc" -I"C:/Users/Ahmed/Documents/GitHub/GOS2022/Projects/PortTest/GOS2022_lib/drv/inc" -I"C:/Users/Ahmed/Documents/GitHub/GOS2022/Projects/PortTest/GOS2022_lib/svl/inc" -I"C:/Users/Ahmed/Documents/GitHub/GOS2022/Projects/PortTest/GOS2022_lib/bld/inc" -I"C:/Users/Ahmed/Documents/GitHub/GOS2022/Projects/PortTest/GOS2022_lib/Inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Driver-2f-cfg

clean-Driver-2f-cfg:
	-$(RM) ./Driver/cfg/dma_cfg.cyclo ./Driver/cfg/dma_cfg.d ./Driver/cfg/dma_cfg.o ./Driver/cfg/dma_cfg.su ./Driver/cfg/dsm_cfg.cyclo ./Driver/cfg/dsm_cfg.d ./Driver/cfg/dsm_cfg.o ./Driver/cfg/dsm_cfg.su ./Driver/cfg/gpio_cfg.cyclo ./Driver/cfg/gpio_cfg.d ./Driver/cfg/gpio_cfg.o ./Driver/cfg/gpio_cfg.su ./Driver/cfg/i2c_cfg.cyclo ./Driver/cfg/i2c_cfg.d ./Driver/cfg/i2c_cfg.o ./Driver/cfg/i2c_cfg.su ./Driver/cfg/it_cfg.cyclo ./Driver/cfg/it_cfg.d ./Driver/cfg/it_cfg.o ./Driver/cfg/it_cfg.su ./Driver/cfg/spi_cfg.cyclo ./Driver/cfg/spi_cfg.d ./Driver/cfg/spi_cfg.o ./Driver/cfg/spi_cfg.su ./Driver/cfg/tmr_cfg.cyclo ./Driver/cfg/tmr_cfg.d ./Driver/cfg/tmr_cfg.o ./Driver/cfg/tmr_cfg.su ./Driver/cfg/uart_cfg.cyclo ./Driver/cfg/uart_cfg.d ./Driver/cfg/uart_cfg.o ./Driver/cfg/uart_cfg.su

.PHONY: clean-Driver-2f-cfg


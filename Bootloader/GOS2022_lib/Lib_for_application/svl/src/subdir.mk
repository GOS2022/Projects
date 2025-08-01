################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../svl/src/svl_dhs.c \
../svl/src/svl_dsm.c \
../svl/src/svl_ers.c \
../svl/src/svl_mdi.c \
../svl/src/svl_pdh.c \
../svl/src/svl_sdh.c \
../svl/src/svl_sysmon.c 

OBJS += \
./svl/src/svl_dhs.o \
./svl/src/svl_dsm.o \
./svl/src/svl_ers.o \
./svl/src/svl_mdi.o \
./svl/src/svl_pdh.o \
./svl/src/svl_sdh.o \
./svl/src/svl_sysmon.o 

C_DEPS += \
./svl/src/svl_dhs.d \
./svl/src/svl_dsm.d \
./svl/src/svl_ers.d \
./svl/src/svl_mdi.d \
./svl/src/svl_pdh.d \
./svl/src/svl_sdh.d \
./svl/src/svl_sysmon.d 


# Each subdirectory must supply rules for building sources it contributes
svl/src/%.o svl/src/%.su svl/src/%.cyclo: ../svl/src/%.c svl/src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DSTM32 -DSTM32F4 -DSTM32F446RETx -DNUCLEO_F446RE -DSTM32F446xx -c -I"C:/Users/Gabor/Documents/GitHub/GOS2022_root/Projects/Bootloader/GOS2022" -I"C:/Users/Gabor/Documents/GitHub/GOS2022_root/Projects/Bootloader/GOS2022/os/kernel/inc" -I"C:/Users/Gabor/Documents/GitHub/GOS2022_root/Projects/Bootloader/GOS2022/os/driver/inc" -I"C:/Users/Gabor/Documents/GitHub/GOS2022_root/Projects/Bootloader/GOS2022/os/services/inc" -I"C:/Users/Gabor/Documents/GitHub/GOS2022_root/Projects/Bootloader/GOS2022_lib/drv/inc" -I"C:/Users/Gabor/Documents/GitHub/GOS2022_root/Projects/Bootloader/GOS2022_lib/bld/inc" -I"C:/Users/Gabor/Documents/GitHub/GOS2022_root/Projects/Bootloader/GOS2022_lib/Inc" -I"C:/Users/Gabor/Documents/GitHub/GOS2022_root/Projects/Bootloader/GOS2022_lib/hal/STM32F4xx_HAL_Driver/Inc" -I"C:/Users/Gabor/Documents/GitHub/GOS2022_root/Projects/Bootloader/GOS2022_lib/hal/STM32F4xx_HAL_Driver/Inc/Legacy" -I"C:/Users/Gabor/Documents/GitHub/GOS2022_root/Projects/Bootloader/GOS2022_lib/hal/CMSIS/Include" -I"C:/Users/Gabor/Documents/GitHub/GOS2022_root/Projects/Bootloader/GOS2022_lib/hal/CMSIS/Device/ST/STM32F4xx/Include" -I"C:/Users/Gabor/Documents/GitHub/GOS2022_root/Projects/Bootloader/GOS2022_lib/svl/inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-svl-2f-src

clean-svl-2f-src:
	-$(RM) ./svl/src/svl_dhs.cyclo ./svl/src/svl_dhs.d ./svl/src/svl_dhs.o ./svl/src/svl_dhs.su ./svl/src/svl_dsm.cyclo ./svl/src/svl_dsm.d ./svl/src/svl_dsm.o ./svl/src/svl_dsm.su ./svl/src/svl_ers.cyclo ./svl/src/svl_ers.d ./svl/src/svl_ers.o ./svl/src/svl_ers.su ./svl/src/svl_mdi.cyclo ./svl/src/svl_mdi.d ./svl/src/svl_mdi.o ./svl/src/svl_mdi.su ./svl/src/svl_pdh.cyclo ./svl/src/svl_pdh.d ./svl/src/svl_pdh.o ./svl/src/svl_pdh.su ./svl/src/svl_sdh.cyclo ./svl/src/svl_sdh.d ./svl/src/svl_sdh.o ./svl/src/svl_sdh.su ./svl/src/svl_sysmon.cyclo ./svl/src/svl_sysmon.d ./svl/src/svl_sysmon.o ./svl/src/svl_sysmon.su

.PHONY: clean-svl-2f-src


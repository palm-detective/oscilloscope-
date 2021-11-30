################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/Bios.c \
../Core/Src/Display.c \
../Core/Src/Draw.c \
../Core/Src/File.c \
../Core/Src/Flash.c \
../Core/Src/Func.c \
../Core/Src/Lcd_ctl.c \
../Core/Src/Menu.c \
../Core/Src/Process.c \
../Core/Src/Touch_ctl.c \
../Core/Src/main.c \
../Core/Src/stm32f4xx_hal_msp.c \
../Core/Src/stm32f4xx_it.c \
../Core/Src/syscalls.c \
../Core/Src/sysmem.c \
../Core/Src/system_stm32f4xx.c \
../Core/Src/w25qxx.c 

OBJS += \
./Core/Src/Bios.o \
./Core/Src/Display.o \
./Core/Src/Draw.o \
./Core/Src/File.o \
./Core/Src/Flash.o \
./Core/Src/Func.o \
./Core/Src/Lcd_ctl.o \
./Core/Src/Menu.o \
./Core/Src/Process.o \
./Core/Src/Touch_ctl.o \
./Core/Src/main.o \
./Core/Src/stm32f4xx_hal_msp.o \
./Core/Src/stm32f4xx_it.o \
./Core/Src/syscalls.o \
./Core/Src/sysmem.o \
./Core/Src/system_stm32f4xx.o \
./Core/Src/w25qxx.o 

C_DEPS += \
./Core/Src/Bios.d \
./Core/Src/Display.d \
./Core/Src/Draw.d \
./Core/Src/File.d \
./Core/Src/Flash.d \
./Core/Src/Func.d \
./Core/Src/Lcd_ctl.d \
./Core/Src/Menu.d \
./Core/Src/Process.d \
./Core/Src/Touch_ctl.d \
./Core/Src/main.d \
./Core/Src/stm32f4xx_hal_msp.d \
./Core/Src/stm32f4xx_it.d \
./Core/Src/syscalls.d \
./Core/Src/sysmem.d \
./Core/Src/system_stm32f4xx.d \
./Core/Src/w25qxx.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/%.o: ../Core/Src/%.c Core/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -c -I../USB_DEVICE/App -I../USB_DEVICE/Target -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/MSC/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../FATFS/Target -I../FATFS/App -I../Middlewares/Third_Party/FatFs/src -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src

clean-Core-2f-Src:
	-$(RM) ./Core/Src/Bios.d ./Core/Src/Bios.o ./Core/Src/Display.d ./Core/Src/Display.o ./Core/Src/Draw.d ./Core/Src/Draw.o ./Core/Src/File.d ./Core/Src/File.o ./Core/Src/Flash.d ./Core/Src/Flash.o ./Core/Src/Func.d ./Core/Src/Func.o ./Core/Src/Lcd_ctl.d ./Core/Src/Lcd_ctl.o ./Core/Src/Menu.d ./Core/Src/Menu.o ./Core/Src/Process.d ./Core/Src/Process.o ./Core/Src/Touch_ctl.d ./Core/Src/Touch_ctl.o ./Core/Src/main.d ./Core/Src/main.o ./Core/Src/stm32f4xx_hal_msp.d ./Core/Src/stm32f4xx_hal_msp.o ./Core/Src/stm32f4xx_it.d ./Core/Src/stm32f4xx_it.o ./Core/Src/syscalls.d ./Core/Src/syscalls.o ./Core/Src/sysmem.d ./Core/Src/sysmem.o ./Core/Src/system_stm32f4xx.d ./Core/Src/system_stm32f4xx.o ./Core/Src/w25qxx.d ./Core/Src/w25qxx.o

.PHONY: clean-Core-2f-Src


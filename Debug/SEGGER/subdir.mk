################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../SEGGER/segger_uart.c 

OBJS += \
./SEGGER/segger_uart.o 

C_DEPS += \
./SEGGER/segger_uart.d 


# Each subdirectory must supply rules for building sources it contributes
SEGGER/segger_uart.o: ../SEGGER/segger_uart.c SEGGER/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DSTM32 -DSTM32F407G_DISC1 -DSTM32F4 -DSTM32F407VGTx -c -I../Inc -I"/home/deviousprophet/STM32CubeIDE/workspace/STM32F407_FreeRTOS/SEGGER/Config" -I"/home/deviousprophet/STM32CubeIDE/workspace/STM32F407_FreeRTOS/SEGGER/OS" -I"/home/deviousprophet/STM32CubeIDE/workspace/STM32F407_FreeRTOS/SEGGER/SEGGER" -I"/home/deviousprophet/STM32CubeIDE/workspace/STM32F407_FreeRTOS/Devices/Inc" -I"/home/deviousprophet/STM32CubeIDE/workspace/STM32F407_FreeRTOS/Libraries/Inc" -I"/home/deviousprophet/STM32CubeIDE/workspace/STM32F407_FreeRTOS/Inc" -I"/home/deviousprophet/STM32CubeIDE/workspace/STM32F407_FreeRTOS/FreeRTOS" -I"/home/deviousprophet/STM32CubeIDE/workspace/STM32F407_FreeRTOS/FreeRTOS/portable/GCC/ARM_CM4F" -I"/home/deviousprophet/STM32CubeIDE/workspace/STM32F407_FreeRTOS/FreeRTOS/include" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"SEGGER/segger_uart.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -mthumb -o "$@"


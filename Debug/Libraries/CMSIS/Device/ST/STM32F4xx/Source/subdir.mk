################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Libraries/CMSIS/Device/ST/STM32F4xx/Source/system_stm32f4xx.c 

OBJS += \
./Libraries/CMSIS/Device/ST/STM32F4xx/Source/system_stm32f4xx.o 

C_DEPS += \
./Libraries/CMSIS/Device/ST/STM32F4xx/Source/system_stm32f4xx.d 


# Each subdirectory must supply rules for building sources it contributes
Libraries/CMSIS/Device/ST/STM32F4xx/Source/system_stm32f4xx.o: ../Libraries/CMSIS/Device/ST/STM32F4xx/Source/system_stm32f4xx.c Libraries/CMSIS/Device/ST/STM32F4xx/Source/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DSTM32 -DSTM32F407G_DISC1 -DSTM32F4 -DSTM32F407VGTx -c -I../Inc -I"/home/deviousprophet/STM32CubeIDE/workspace/STM32F407_FreeRTOS/Inc" -I"/home/deviousprophet/STM32CubeIDE/workspace/STM32F407_FreeRTOS/FreeRTOS" -I"/home/deviousprophet/STM32CubeIDE/workspace/STM32F407_FreeRTOS/FreeRTOS/portable/GCC/ARM_CM4F" -I"/home/deviousprophet/STM32CubeIDE/workspace/STM32F407_FreeRTOS/FreeRTOS/include" -I"/home/deviousprophet/STM32CubeIDE/workspace/STM32F407_FreeRTOS/Libraries/STM32F4xx_StdPeriph_Driver/inc" -I"/home/deviousprophet/STM32CubeIDE/workspace/STM32F407_FreeRTOS/Libraries/CMSIS/Include" -I"/home/deviousprophet/STM32CubeIDE/workspace/STM32F407_FreeRTOS/Libraries/CMSIS/Device/ST/STM32F4xx/Include" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Libraries/CMSIS/Device/ST/STM32F4xx/Source/system_stm32f4xx.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"


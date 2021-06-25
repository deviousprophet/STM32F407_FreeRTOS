################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Libraries/STM32F4xx_StdPeriph_Driver/src/misc.c \
../Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_exti.c \
../Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_gpio.c \
../Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_rcc.c \
../Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_syscfg.c 

OBJS += \
./Libraries/STM32F4xx_StdPeriph_Driver/src/misc.o \
./Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_exti.o \
./Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_gpio.o \
./Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_rcc.o \
./Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_syscfg.o 

C_DEPS += \
./Libraries/STM32F4xx_StdPeriph_Driver/src/misc.d \
./Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_exti.d \
./Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_gpio.d \
./Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_rcc.d \
./Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_syscfg.d 


# Each subdirectory must supply rules for building sources it contributes
Libraries/STM32F4xx_StdPeriph_Driver/src/misc.o: ../Libraries/STM32F4xx_StdPeriph_Driver/src/misc.c Libraries/STM32F4xx_StdPeriph_Driver/src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DSTM32 -DSTM32F407G_DISC1 -DSTM32F4 -DSTM32F407VGTx -c -I../Inc -I"/home/deviousprophet/STM32CubeIDE/workspace/STM32F407_FreeRTOS/Inc" -I"/home/deviousprophet/STM32CubeIDE/workspace/STM32F407_FreeRTOS/FreeRTOS" -I"/home/deviousprophet/STM32CubeIDE/workspace/STM32F407_FreeRTOS/FreeRTOS/portable/GCC/ARM_CM4F" -I"/home/deviousprophet/STM32CubeIDE/workspace/STM32F407_FreeRTOS/FreeRTOS/include" -I"/home/deviousprophet/STM32CubeIDE/workspace/STM32F407_FreeRTOS/Libraries/STM32F4xx_StdPeriph_Driver/inc" -I"/home/deviousprophet/STM32CubeIDE/workspace/STM32F407_FreeRTOS/Libraries/CMSIS/Include" -I"/home/deviousprophet/STM32CubeIDE/workspace/STM32F407_FreeRTOS/Libraries/CMSIS/Device/ST/STM32F4xx/Include" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Libraries/STM32F4xx_StdPeriph_Driver/src/misc.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_exti.o: ../Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_exti.c Libraries/STM32F4xx_StdPeriph_Driver/src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DSTM32 -DSTM32F407G_DISC1 -DSTM32F4 -DSTM32F407VGTx -c -I../Inc -I"/home/deviousprophet/STM32CubeIDE/workspace/STM32F407_FreeRTOS/Inc" -I"/home/deviousprophet/STM32CubeIDE/workspace/STM32F407_FreeRTOS/FreeRTOS" -I"/home/deviousprophet/STM32CubeIDE/workspace/STM32F407_FreeRTOS/FreeRTOS/portable/GCC/ARM_CM4F" -I"/home/deviousprophet/STM32CubeIDE/workspace/STM32F407_FreeRTOS/FreeRTOS/include" -I"/home/deviousprophet/STM32CubeIDE/workspace/STM32F407_FreeRTOS/Libraries/STM32F4xx_StdPeriph_Driver/inc" -I"/home/deviousprophet/STM32CubeIDE/workspace/STM32F407_FreeRTOS/Libraries/CMSIS/Include" -I"/home/deviousprophet/STM32CubeIDE/workspace/STM32F407_FreeRTOS/Libraries/CMSIS/Device/ST/STM32F4xx/Include" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_exti.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_gpio.o: ../Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_gpio.c Libraries/STM32F4xx_StdPeriph_Driver/src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DSTM32 -DSTM32F407G_DISC1 -DSTM32F4 -DSTM32F407VGTx -c -I../Inc -I"/home/deviousprophet/STM32CubeIDE/workspace/STM32F407_FreeRTOS/Inc" -I"/home/deviousprophet/STM32CubeIDE/workspace/STM32F407_FreeRTOS/FreeRTOS" -I"/home/deviousprophet/STM32CubeIDE/workspace/STM32F407_FreeRTOS/FreeRTOS/portable/GCC/ARM_CM4F" -I"/home/deviousprophet/STM32CubeIDE/workspace/STM32F407_FreeRTOS/FreeRTOS/include" -I"/home/deviousprophet/STM32CubeIDE/workspace/STM32F407_FreeRTOS/Libraries/STM32F4xx_StdPeriph_Driver/inc" -I"/home/deviousprophet/STM32CubeIDE/workspace/STM32F407_FreeRTOS/Libraries/CMSIS/Include" -I"/home/deviousprophet/STM32CubeIDE/workspace/STM32F407_FreeRTOS/Libraries/CMSIS/Device/ST/STM32F4xx/Include" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_gpio.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_rcc.o: ../Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_rcc.c Libraries/STM32F4xx_StdPeriph_Driver/src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DSTM32 -DSTM32F407G_DISC1 -DSTM32F4 -DSTM32F407VGTx -c -I../Inc -I"/home/deviousprophet/STM32CubeIDE/workspace/STM32F407_FreeRTOS/Inc" -I"/home/deviousprophet/STM32CubeIDE/workspace/STM32F407_FreeRTOS/FreeRTOS" -I"/home/deviousprophet/STM32CubeIDE/workspace/STM32F407_FreeRTOS/FreeRTOS/portable/GCC/ARM_CM4F" -I"/home/deviousprophet/STM32CubeIDE/workspace/STM32F407_FreeRTOS/FreeRTOS/include" -I"/home/deviousprophet/STM32CubeIDE/workspace/STM32F407_FreeRTOS/Libraries/STM32F4xx_StdPeriph_Driver/inc" -I"/home/deviousprophet/STM32CubeIDE/workspace/STM32F407_FreeRTOS/Libraries/CMSIS/Include" -I"/home/deviousprophet/STM32CubeIDE/workspace/STM32F407_FreeRTOS/Libraries/CMSIS/Device/ST/STM32F4xx/Include" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_rcc.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_syscfg.o: ../Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_syscfg.c Libraries/STM32F4xx_StdPeriph_Driver/src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DSTM32 -DSTM32F407G_DISC1 -DSTM32F4 -DSTM32F407VGTx -c -I../Inc -I"/home/deviousprophet/STM32CubeIDE/workspace/STM32F407_FreeRTOS/Inc" -I"/home/deviousprophet/STM32CubeIDE/workspace/STM32F407_FreeRTOS/FreeRTOS" -I"/home/deviousprophet/STM32CubeIDE/workspace/STM32F407_FreeRTOS/FreeRTOS/portable/GCC/ARM_CM4F" -I"/home/deviousprophet/STM32CubeIDE/workspace/STM32F407_FreeRTOS/FreeRTOS/include" -I"/home/deviousprophet/STM32CubeIDE/workspace/STM32F407_FreeRTOS/Libraries/STM32F4xx_StdPeriph_Driver/inc" -I"/home/deviousprophet/STM32CubeIDE/workspace/STM32F407_FreeRTOS/Libraries/CMSIS/Include" -I"/home/deviousprophet/STM32CubeIDE/workspace/STM32F407_FreeRTOS/Libraries/CMSIS/Device/ST/STM32F4xx/Include" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_syscfg.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"


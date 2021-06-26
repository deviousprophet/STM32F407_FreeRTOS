################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Devices/Src/ade7753.c \
../Devices/Src/ds1307.c \
../Devices/Src/keypad.c \
../Devices/Src/lcd5110.c 

OBJS += \
./Devices/Src/ade7753.o \
./Devices/Src/ds1307.o \
./Devices/Src/keypad.o \
./Devices/Src/lcd5110.o 

C_DEPS += \
./Devices/Src/ade7753.d \
./Devices/Src/ds1307.d \
./Devices/Src/keypad.d \
./Devices/Src/lcd5110.d 


# Each subdirectory must supply rules for building sources it contributes
Devices/Src/ade7753.o: ../Devices/Src/ade7753.c Devices/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DSTM32 -DSTM32F407G_DISC1 -DSTM32F4 -DSTM32F407VGTx -c -I../Inc -I"/home/deviousprophet/STM32CubeIDE/workspace/STM32F407_FreeRTOS/Devices/Inc" -I"/home/deviousprophet/STM32CubeIDE/workspace/STM32F407_FreeRTOS/Libraries/Inc" -I"/home/deviousprophet/STM32CubeIDE/workspace/STM32F407_FreeRTOS/Inc" -I"/home/deviousprophet/STM32CubeIDE/workspace/STM32F407_FreeRTOS/FreeRTOS" -I"/home/deviousprophet/STM32CubeIDE/workspace/STM32F407_FreeRTOS/FreeRTOS/portable/GCC/ARM_CM4F" -I"/home/deviousprophet/STM32CubeIDE/workspace/STM32F407_FreeRTOS/FreeRTOS/include" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Devices/Src/ade7753.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -mthumb -o "$@"
Devices/Src/ds1307.o: ../Devices/Src/ds1307.c Devices/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DSTM32 -DSTM32F407G_DISC1 -DSTM32F4 -DSTM32F407VGTx -c -I../Inc -I"/home/deviousprophet/STM32CubeIDE/workspace/STM32F407_FreeRTOS/Devices/Inc" -I"/home/deviousprophet/STM32CubeIDE/workspace/STM32F407_FreeRTOS/Libraries/Inc" -I"/home/deviousprophet/STM32CubeIDE/workspace/STM32F407_FreeRTOS/Inc" -I"/home/deviousprophet/STM32CubeIDE/workspace/STM32F407_FreeRTOS/FreeRTOS" -I"/home/deviousprophet/STM32CubeIDE/workspace/STM32F407_FreeRTOS/FreeRTOS/portable/GCC/ARM_CM4F" -I"/home/deviousprophet/STM32CubeIDE/workspace/STM32F407_FreeRTOS/FreeRTOS/include" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Devices/Src/ds1307.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -mthumb -o "$@"
Devices/Src/keypad.o: ../Devices/Src/keypad.c Devices/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DSTM32 -DSTM32F407G_DISC1 -DSTM32F4 -DSTM32F407VGTx -c -I../Inc -I"/home/deviousprophet/STM32CubeIDE/workspace/STM32F407_FreeRTOS/Devices/Inc" -I"/home/deviousprophet/STM32CubeIDE/workspace/STM32F407_FreeRTOS/Libraries/Inc" -I"/home/deviousprophet/STM32CubeIDE/workspace/STM32F407_FreeRTOS/Inc" -I"/home/deviousprophet/STM32CubeIDE/workspace/STM32F407_FreeRTOS/FreeRTOS" -I"/home/deviousprophet/STM32CubeIDE/workspace/STM32F407_FreeRTOS/FreeRTOS/portable/GCC/ARM_CM4F" -I"/home/deviousprophet/STM32CubeIDE/workspace/STM32F407_FreeRTOS/FreeRTOS/include" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Devices/Src/keypad.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -mthumb -o "$@"
Devices/Src/lcd5110.o: ../Devices/Src/lcd5110.c Devices/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DSTM32 -DSTM32F407G_DISC1 -DSTM32F4 -DSTM32F407VGTx -c -I../Inc -I"/home/deviousprophet/STM32CubeIDE/workspace/STM32F407_FreeRTOS/Devices/Inc" -I"/home/deviousprophet/STM32CubeIDE/workspace/STM32F407_FreeRTOS/Libraries/Inc" -I"/home/deviousprophet/STM32CubeIDE/workspace/STM32F407_FreeRTOS/Inc" -I"/home/deviousprophet/STM32CubeIDE/workspace/STM32F407_FreeRTOS/FreeRTOS" -I"/home/deviousprophet/STM32CubeIDE/workspace/STM32F407_FreeRTOS/FreeRTOS/portable/GCC/ARM_CM4F" -I"/home/deviousprophet/STM32CubeIDE/workspace/STM32F407_FreeRTOS/FreeRTOS/include" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Devices/Src/lcd5110.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -mthumb -o "$@"


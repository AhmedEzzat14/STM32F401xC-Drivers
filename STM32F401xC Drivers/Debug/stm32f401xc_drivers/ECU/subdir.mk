################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../stm32f401xc_drivers/ECU/DAC_driver.c \
../stm32f401xc_drivers/ECU/IR_driver.c \
../stm32f401xc_drivers/ECU/Led_Matrix_driver.c \
../stm32f401xc_drivers/ECU/MPU6050_Driver.c \
../stm32f401xc_drivers/ECU/Serial2Parallel_drivers.c \
../stm32f401xc_drivers/ECU/SevenSegment_driver.c \
../stm32f401xc_drivers/ECU/TFT_driver.c \
../stm32f401xc_drivers/ECU/Ultrasonic_driver.c \
../stm32f401xc_drivers/ECU/nrf24l01_driver.c 

OBJS += \
./stm32f401xc_drivers/ECU/DAC_driver.o \
./stm32f401xc_drivers/ECU/IR_driver.o \
./stm32f401xc_drivers/ECU/Led_Matrix_driver.o \
./stm32f401xc_drivers/ECU/MPU6050_Driver.o \
./stm32f401xc_drivers/ECU/Serial2Parallel_drivers.o \
./stm32f401xc_drivers/ECU/SevenSegment_driver.o \
./stm32f401xc_drivers/ECU/TFT_driver.o \
./stm32f401xc_drivers/ECU/Ultrasonic_driver.o \
./stm32f401xc_drivers/ECU/nrf24l01_driver.o 

C_DEPS += \
./stm32f401xc_drivers/ECU/DAC_driver.d \
./stm32f401xc_drivers/ECU/IR_driver.d \
./stm32f401xc_drivers/ECU/Led_Matrix_driver.d \
./stm32f401xc_drivers/ECU/MPU6050_Driver.d \
./stm32f401xc_drivers/ECU/Serial2Parallel_drivers.d \
./stm32f401xc_drivers/ECU/SevenSegment_driver.d \
./stm32f401xc_drivers/ECU/TFT_driver.d \
./stm32f401xc_drivers/ECU/Ultrasonic_driver.d \
./stm32f401xc_drivers/ECU/nrf24l01_driver.d 


# Each subdirectory must supply rules for building sources it contributes
stm32f401xc_drivers/ECU/%.o stm32f401xc_drivers/ECU/%.su stm32f401xc_drivers/ECU/%.cyclo: ../stm32f401xc_drivers/ECU/%.c stm32f401xc_drivers/ECU/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DSTM32F401CCUx -DSTM32 -DSTM32F4 -c -I../Inc -I"D:/Electric Programs/Embeded Systems/LearnInDepth/Boards Drivers/STM32F401xC Drivers/STM32F401xC Drivers/stm32f401xc_drivers" -I"D:/Electric Programs/Embeded Systems/LearnInDepth/Boards Drivers/STM32F401xC Drivers/STM32F401xC Drivers/stm32f401xc_drivers/Inc" -I"D:/Electric Programs/Embeded Systems/LearnInDepth/Boards Drivers/STM32F401xC Drivers/STM32F401xC Drivers/stm32f401xc_drivers/ECU/Inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-stm32f401xc_drivers-2f-ECU

clean-stm32f401xc_drivers-2f-ECU:
	-$(RM) ./stm32f401xc_drivers/ECU/DAC_driver.cyclo ./stm32f401xc_drivers/ECU/DAC_driver.d ./stm32f401xc_drivers/ECU/DAC_driver.o ./stm32f401xc_drivers/ECU/DAC_driver.su ./stm32f401xc_drivers/ECU/IR_driver.cyclo ./stm32f401xc_drivers/ECU/IR_driver.d ./stm32f401xc_drivers/ECU/IR_driver.o ./stm32f401xc_drivers/ECU/IR_driver.su ./stm32f401xc_drivers/ECU/Led_Matrix_driver.cyclo ./stm32f401xc_drivers/ECU/Led_Matrix_driver.d ./stm32f401xc_drivers/ECU/Led_Matrix_driver.o ./stm32f401xc_drivers/ECU/Led_Matrix_driver.su ./stm32f401xc_drivers/ECU/MPU6050_Driver.cyclo ./stm32f401xc_drivers/ECU/MPU6050_Driver.d ./stm32f401xc_drivers/ECU/MPU6050_Driver.o ./stm32f401xc_drivers/ECU/MPU6050_Driver.su ./stm32f401xc_drivers/ECU/Serial2Parallel_drivers.cyclo ./stm32f401xc_drivers/ECU/Serial2Parallel_drivers.d ./stm32f401xc_drivers/ECU/Serial2Parallel_drivers.o ./stm32f401xc_drivers/ECU/Serial2Parallel_drivers.su ./stm32f401xc_drivers/ECU/SevenSegment_driver.cyclo ./stm32f401xc_drivers/ECU/SevenSegment_driver.d ./stm32f401xc_drivers/ECU/SevenSegment_driver.o ./stm32f401xc_drivers/ECU/SevenSegment_driver.su ./stm32f401xc_drivers/ECU/TFT_driver.cyclo ./stm32f401xc_drivers/ECU/TFT_driver.d ./stm32f401xc_drivers/ECU/TFT_driver.o ./stm32f401xc_drivers/ECU/TFT_driver.su ./stm32f401xc_drivers/ECU/Ultrasonic_driver.cyclo ./stm32f401xc_drivers/ECU/Ultrasonic_driver.d ./stm32f401xc_drivers/ECU/Ultrasonic_driver.o ./stm32f401xc_drivers/ECU/Ultrasonic_driver.su ./stm32f401xc_drivers/ECU/nrf24l01_driver.cyclo ./stm32f401xc_drivers/ECU/nrf24l01_driver.d ./stm32f401xc_drivers/ECU/nrf24l01_driver.o ./stm32f401xc_drivers/ECU/nrf24l01_driver.su

.PHONY: clean-stm32f401xc_drivers-2f-ECU


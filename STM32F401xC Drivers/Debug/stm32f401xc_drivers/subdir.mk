################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../stm32f401xc_drivers/stm32f401xc_EXTI_driver.c \
../stm32f401xc_drivers/stm32f401xc_gpio_driver.c 

OBJS += \
./stm32f401xc_drivers/stm32f401xc_EXTI_driver.o \
./stm32f401xc_drivers/stm32f401xc_gpio_driver.o 

C_DEPS += \
./stm32f401xc_drivers/stm32f401xc_EXTI_driver.d \
./stm32f401xc_drivers/stm32f401xc_gpio_driver.d 


# Each subdirectory must supply rules for building sources it contributes
stm32f401xc_drivers/%.o stm32f401xc_drivers/%.su stm32f401xc_drivers/%.cyclo: ../stm32f401xc_drivers/%.c stm32f401xc_drivers/subdir.mk
	arm-none-eabi-gcc -gdwarf-2 "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DSTM32F401CCUx -DSTM32 -DSTM32F4 -c -I../Inc -I"D:/Electric Programs/Embeded Systems/LearnInDepth/STM32F401xC Drivers/stm32f401xc_drivers/ECU/Inc" -I"D:/Electric Programs/Embeded Systems/LearnInDepth/STM32F401xC Drivers/stm32f401xc_drivers/Inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-stm32f401xc_drivers

clean-stm32f401xc_drivers:
	-$(RM) ./stm32f401xc_drivers/stm32f401xc_EXTI_driver.cyclo ./stm32f401xc_drivers/stm32f401xc_EXTI_driver.d ./stm32f401xc_drivers/stm32f401xc_EXTI_driver.o ./stm32f401xc_drivers/stm32f401xc_EXTI_driver.su ./stm32f401xc_drivers/stm32f401xc_gpio_driver.cyclo ./stm32f401xc_drivers/stm32f401xc_gpio_driver.d ./stm32f401xc_drivers/stm32f401xc_gpio_driver.o ./stm32f401xc_drivers/stm32f401xc_gpio_driver.su

.PHONY: clean-stm32f401xc_drivers


################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/adc_lib/ad717x/ad717x.c \
../Core/Src/adc_lib/ad717x/no_os_alloc.c \
../Core/Src/adc_lib/ad717x/no_os_util.c 

OBJS += \
./Core/Src/adc_lib/ad717x/ad717x.o \
./Core/Src/adc_lib/ad717x/no_os_alloc.o \
./Core/Src/adc_lib/ad717x/no_os_util.o 

C_DEPS += \
./Core/Src/adc_lib/ad717x/ad717x.d \
./Core/Src/adc_lib/ad717x/no_os_alloc.d \
./Core/Src/adc_lib/ad717x/no_os_util.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/adc_lib/ad717x/%.o Core/Src/adc_lib/ad717x/%.su Core/Src/adc_lib/ad717x/%.cyclo: ../Core/Src/adc_lib/ad717x/%.c Core/Src/adc_lib/ad717x/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m33 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32U575xx -c -I../Core/Inc -I"C:/Users/Alex/Sigurd/Firmware/Core/Src/adc_lib/ad717x" -I../Drivers/STM32U5xx_HAL_Driver/Inc -I../Drivers/STM32U5xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32U5xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-adc_lib-2f-ad717x

clean-Core-2f-Src-2f-adc_lib-2f-ad717x:
	-$(RM) ./Core/Src/adc_lib/ad717x/ad717x.cyclo ./Core/Src/adc_lib/ad717x/ad717x.d ./Core/Src/adc_lib/ad717x/ad717x.o ./Core/Src/adc_lib/ad717x/ad717x.su ./Core/Src/adc_lib/ad717x/no_os_alloc.cyclo ./Core/Src/adc_lib/ad717x/no_os_alloc.d ./Core/Src/adc_lib/ad717x/no_os_alloc.o ./Core/Src/adc_lib/ad717x/no_os_alloc.su ./Core/Src/adc_lib/ad717x/no_os_util.cyclo ./Core/Src/adc_lib/ad717x/no_os_util.d ./Core/Src/adc_lib/ad717x/no_os_util.o ./Core/Src/adc_lib/ad717x/no_os_util.su

.PHONY: clean-Core-2f-Src-2f-adc_lib-2f-ad717x


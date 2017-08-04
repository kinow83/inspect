################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/rtx_modules/wifi_rtx.c 

O_SRCS += \
../src/rtx_modules/wifi_rtx.o 

OBJS += \
./src/rtx_modules/wifi_rtx.o 

C_DEPS += \
./src/rtx_modules/wifi_rtx.d 


# Each subdirectory must supply rules for building sources it contributes
src/rtx_modules/%.o: ../src/rtx_modules/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



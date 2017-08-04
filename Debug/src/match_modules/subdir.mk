################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/match_modules/wifi_match.c 

O_SRCS += \
../src/match_modules/wifi_match.o 

OBJS += \
./src/match_modules/wifi_match.o 

C_DEPS += \
./src/match_modules/wifi_match.d 


# Each subdirectory must supply rules for building sources it contributes
src/match_modules/%.o: ../src/match_modules/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



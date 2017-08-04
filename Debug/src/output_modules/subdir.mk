################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/output_modules/error_output.c \
../src/output_modules/wifi_xml_output.c 

O_SRCS += \
../src/output_modules/error_output.o \
../src/output_modules/wifi_xml_output.o 

OBJS += \
./src/output_modules/error_output.o \
./src/output_modules/wifi_xml_output.o 

C_DEPS += \
./src/output_modules/error_output.d \
./src/output_modules/wifi_xml_output.d 


# Each subdirectory must supply rules for building sources it contributes
src/output_modules/%.o: ../src/output_modules/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



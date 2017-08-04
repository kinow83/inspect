################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/parser_modules/xml_parser.c 

O_SRCS += \
../src/parser_modules/xml_parser.o 

OBJS += \
./src/parser_modules/xml_parser.o 

C_DEPS += \
./src/parser_modules/xml_parser.d 


# Each subdirectory must supply rules for building sources it contributes
src/parser_modules/%.o: ../src/parser_modules/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



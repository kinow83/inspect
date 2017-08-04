################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/osdep/radiotap/parse.c \
../src/osdep/radiotap/radiotap.c 

O_SRCS += \
../src/osdep/radiotap/radiotap.o 

OBJS += \
./src/osdep/radiotap/parse.o \
./src/osdep/radiotap/radiotap.o 

C_DEPS += \
./src/osdep/radiotap/parse.d \
./src/osdep/radiotap/radiotap.d 


# Each subdirectory must supply rules for building sources it contributes
src/osdep/radiotap/%.o: ../src/osdep/radiotap/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



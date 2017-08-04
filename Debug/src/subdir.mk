################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/alloc.c \
../src/convert.c \
../src/inspect.c \
../src/log.c \
../src/match.c \
../src/module.c \
../src/output.c \
../src/parser.c \
../src/resource.c \
../src/rtx.c \
../src/strings.c \
../src/test.c 

O_SRCS += \
../src/inspect-alloc.o \
../src/inspect-convert.o \
../src/inspect-inspect.o \
../src/inspect-log.o \
../src/inspect-match.o \
../src/inspect-module.o \
../src/inspect-output.o \
../src/inspect-parser.o \
../src/inspect-resource.o \
../src/inspect-rtx.o \
../src/inspect-strings.o 

OBJS += \
./src/alloc.o \
./src/convert.o \
./src/inspect.o \
./src/log.o \
./src/match.o \
./src/module.o \
./src/output.o \
./src/parser.o \
./src/resource.o \
./src/rtx.o \
./src/strings.o \
./src/test.o 

C_DEPS += \
./src/alloc.d \
./src/convert.d \
./src/inspect.d \
./src/log.d \
./src/match.d \
./src/module.d \
./src/output.d \
./src/parser.d \
./src/resource.d \
./src/rtx.d \
./src/strings.d \
./src/test.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



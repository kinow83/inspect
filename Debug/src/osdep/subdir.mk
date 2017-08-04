################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/osdep/airpcap.c \
../src/osdep/common.c \
../src/osdep/cygwin.c \
../src/osdep/cygwin_tap.c \
../src/osdep/darwin.c \
../src/osdep/darwin_tap.c \
../src/osdep/dummy.c \
../src/osdep/dummy_tap.c \
../src/osdep/file.c \
../src/osdep/freebsd.c \
../src/osdep/freebsd_tap.c \
../src/osdep/linux.c \
../src/osdep/linux_tap.c \
../src/osdep/netbsd.c \
../src/osdep/netbsd_tap.c \
../src/osdep/network.c \
../src/osdep/openbsd.c \
../src/osdep/openbsd_tap.c \
../src/osdep/osdep.c 

O_SRCS += \
../src/osdep/common.o \
../src/osdep/file.o \
../src/osdep/linux.o \
../src/osdep/linux_tap.o \
../src/osdep/network.o \
../src/osdep/osdep.o 

OBJS += \
./src/osdep/airpcap.o \
./src/osdep/common.o \
./src/osdep/cygwin.o \
./src/osdep/cygwin_tap.o \
./src/osdep/darwin.o \
./src/osdep/darwin_tap.o \
./src/osdep/dummy.o \
./src/osdep/dummy_tap.o \
./src/osdep/file.o \
./src/osdep/freebsd.o \
./src/osdep/freebsd_tap.o \
./src/osdep/linux.o \
./src/osdep/linux_tap.o \
./src/osdep/netbsd.o \
./src/osdep/netbsd_tap.o \
./src/osdep/network.o \
./src/osdep/openbsd.o \
./src/osdep/openbsd_tap.o \
./src/osdep/osdep.o 

C_DEPS += \
./src/osdep/airpcap.d \
./src/osdep/common.d \
./src/osdep/cygwin.d \
./src/osdep/cygwin_tap.d \
./src/osdep/darwin.d \
./src/osdep/darwin_tap.d \
./src/osdep/dummy.d \
./src/osdep/dummy_tap.d \
./src/osdep/file.d \
./src/osdep/freebsd.d \
./src/osdep/freebsd_tap.d \
./src/osdep/linux.d \
./src/osdep/linux_tap.d \
./src/osdep/netbsd.d \
./src/osdep/netbsd_tap.d \
./src/osdep/network.d \
./src/osdep/openbsd.d \
./src/osdep/openbsd_tap.d \
./src/osdep/osdep.d 


# Each subdirectory must supply rules for building sources it contributes
src/osdep/%.o: ../src/osdep/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



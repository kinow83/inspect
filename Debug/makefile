################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/rtx_modules/subdir.mk
-include src/parser_modules/subdir.mk
-include src/output_modules/subdir.mk
-include src/osdep/radiotap/subdir.mk
-include src/osdep/subdir.mk
-include src/match_modules/subdir.mk
-include src/ezxml/subdir.mk
-include src/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: inspect

# Tool invocations
inspect: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C Linker'
	gcc  -o "inspect" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(EXECUTABLES)$(OBJS)$(C_DEPS) inspect
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets

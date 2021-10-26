################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include Drivers/STM32F4xx_HAL_Driver/Src/subdir.mk
-include Core/Startup/subdir.mk
-include Core/Src/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
SpaceInvaders.elf \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
SpaceInvaders.list \

OBJCOPY_BIN += \
SpaceInvaders.bin \


# All Target
all: SpaceInvaders.elf secondary-outputs

# Tool invocations
SpaceInvaders.elf: $(OBJS) $(USER_OBJS) D:\Univsersity\2020-2nd\ Year\ S2\Comp\ Systems\SpaceInvaders\STM32F411VETX_FLASH.ld
	arm-none-eabi-gcc -o "SpaceInvaders.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m4 -T"D:\Univsersity\2020-2nd Year S2\Comp Systems\SpaceInvaders\STM32F411VETX_FLASH.ld" --specs=nosys.specs -Wl,-Map="SpaceInvaders.map" -Wl,--gc-sections -static --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

SpaceInvaders.list: $(EXECUTABLES)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "SpaceInvaders.list"
	@echo 'Finished building: $@'
	@echo ' '

SpaceInvaders.bin: $(EXECUTABLES)
	arm-none-eabi-objcopy  -O binary $(EXECUTABLES) "SpaceInvaders.bin"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) *
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST) $(OBJCOPY_BIN)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets

################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir.mk
-include arduino/variant/subdir.mk
-include arduino/core/avr/subdir.mk
-include arduino/core/USB/subdir.mk
-include arduino/core/subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
AR += \
C:/AndroXStudio/workspace/ROBOEX_3WD_BUZZER/Release/arduino.ar \

ELF += \
ROBOEX_3WD_BUZZER.elf \

EEP += \
ROBOEX_3WD_BUZZER.eep \

FLASH_IMAGE += \
ROBOEX_3WD_BUZZER.bin \


# All Target
all: ROBOEX_3WD_BUZZER

# Tool invocations
ROBOEX_3WD_BUZZER: $(FLASH_IMAGE)
	@echo 'Building target: $@'
	@echo 'Printing size:'
	"C:/AndroXStudio/legacy/arduino/hardware/tools/g++_arm_none_eabi/bin/arm-none-eabi-size" -A "C:/AndroXStudio/workspace/ROBOEX_3WD_BUZZER/Release/ROBOEX_3WD_BUZZER.elf"
	@echo 'Finished building target: $@'
	@echo ' '

C:/AndroXStudio/workspace/ROBOEX_3WD_BUZZER/Release/arduino.ar: $(AR_OBJ)
	@echo 'Starting archiver'
	"C:/AndroXStudio/legacy/arduino/hardware/tools/g++_arm_none_eabi/bin/arm-none-eabi-ar" rcs "C:/AndroXStudio/workspace/ROBOEX_3WD_BUZZER/Release/arduino.ar"   $(AR_OBJ)
	@echo 'Finished building: $@'
	@echo ' '

ROBOEX_3WD_BUZZER.elf: $(LINK_OBJ) $(AR)
	@echo 'Starting combiner'
	"C:/AndroXStudio/legacy/arduino/hardware/tools/g++_arm_none_eabi/bin/arm-none-eabi-g++" -Os -Wl,--gc-sections -mcpu=cortex-m3 "-T/C:/AndroXStudio/legacy/arduino/hardware/arduino/sam/variants/arduino_due_x/linker_scripts/gcc/flash.ld" "-Wl,-Map,C:/AndroXStudio/workspace/ROBOEX_3WD_BUZZER/Release/ROBOEX_3WD_BUZZER.map" -o "C:/AndroXStudio/workspace/ROBOEX_3WD_BUZZER/Release/ROBOEX_3WD_BUZZER.elf" "-LC:/AndroXStudio/workspace/ROBOEX_3WD_BUZZER/Release" -lm -lgcc -mthumb -Wl,--cref -Wl,--check-sections -Wl,--gc-sections -Wl,--entry=Reset_Handler -Wl,--unresolved-symbols=report-all -Wl,--warn-common -Wl,--warn-section-align -Wl,--warn-unresolved-symbols -Wl,--start-group "C:/AndroXStudio/workspace/ROBOEX_3WD_BUZZER/Release/arduino/syscalls_sam3.c.o"   $(LINK_OBJ) $(AR)  "/C:/AndroXStudio/legacy/arduino/hardware/arduino/sam/variants/arduino_due_x/libsam_sam3x8e_gcc_rel.a" "C:/AndroXStudio/workspace/ROBOEX_3WD_BUZZER/Release/arduino.ar" -Wl,--end-group
	@echo 'Finished building: $@'
	@echo ' '

ROBOEX_3WD_BUZZER.eep: $(ELF)
	@echo 'Create eeprom image'
	${A.RECIPE.OBJCOPY.EEP.PATTERN}
	@echo 'Finished building: $@'
	@echo ' '

ROBOEX_3WD_BUZZER.bin: $(EEP)
	@echo 'Create Flash image (ihex format)'
	"C:/AndroXStudio/legacy/arduino/hardware/tools/g++_arm_none_eabi/bin/arm-none-eabi-objcopy" -O binary "C:/AndroXStudio/workspace/ROBOEX_3WD_BUZZER/Release/ROBOEX_3WD_BUZZER.elf" "C:/AndroXStudio/workspace/ROBOEX_3WD_BUZZER/Release/ROBOEX_3WD_BUZZER.bin"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(C_DEPS)$(AR)$(FLASH_IMAGE)$(EEP)$(CXX_DEPS)$(C++_DEPS)$(ELF)$(CC_DEPS)$(AR_OBJ)$(CPP_DEPS)$(LINK_OBJ)$(C_UPPER_DEPS)$(SIZEDUMMY) ROBOEX_3WD_BUZZER
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets

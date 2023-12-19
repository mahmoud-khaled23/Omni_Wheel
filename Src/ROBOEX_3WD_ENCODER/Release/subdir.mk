################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../3WD_MOTOR_LIB.cpp \
../ROBOEX_3WD_ENCODER.cpp \
../TEXT_LCD.cpp 

CPP_DEPS += \
./3WD_MOTOR_LIB.cpp.d \
./ROBOEX_3WD_ENCODER.cpp.d \
./TEXT_LCD.cpp.d 

LINK_OBJ += \
./3WD_MOTOR_LIB.cpp.o \
./ROBOEX_3WD_ENCODER.cpp.o \
./TEXT_LCD.cpp.o 


# Each subdirectory must supply rules for building sources it contributes
3WD_MOTOR_LIB.cpp.o: ../3WD_MOTOR_LIB.cpp
	@echo 'Building file: $<'
	@echo 'Starting C++ compile'
	"C:/AndroXStudio/legacy/arduino/hardware/tools/g++_arm_none_eabi/bin/arm-none-eabi-g++" -c -g -Os -ffunction-sections -fdata-sections -nostdlib --param max-inline-insns-single=500 -fno-rtti -fno-exceptions -Dprintf=iprintf -mcpu=cortex-m3 -DF_CPU=84000000L -DARDUINO=155-r2 -DARDUINO_SAM_DUE -DARDUINO_ARCH_SAM -D__SAM3X8E__ -mthumb -DUSB_VID=0x2341 -DUSB_PID=0x003e -DUSBCON '-DUSB_MANUFACTURER="Unknown"' '-DUSB_PRODUCT="Arduino Due"' "-IC:/AndroXStudio/legacy/arduino/hardware/arduino/sam/system/libsam" "-IC:/AndroXStudio/legacy/arduino/hardware/arduino/sam/system/CMSIS/CMSIS/Include/" "-IC:/AndroXStudio/legacy/arduino/hardware/arduino/sam/system/CMSIS/Device/ATMEL/"   -I"C:\AndroXStudio\legacy\arduino\hardware\arduino\sam\cores\arduino" -I"C:\AndroXStudio\legacy\arduino\hardware\arduino\sam\variants\arduino_due_x" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -D__IN_ECLIPSE__=1 -x c++ "$<"  -o  "$@"   -w
	@echo 'Finished building: $<'
	@echo ' '

ROBOEX_3WD_ENCODER.cpp.o: ../ROBOEX_3WD_ENCODER.cpp
	@echo 'Building file: $<'
	@echo 'Starting C++ compile'
	"C:/AndroXStudio/legacy/arduino/hardware/tools/g++_arm_none_eabi/bin/arm-none-eabi-g++" -c -g -Os -ffunction-sections -fdata-sections -nostdlib --param max-inline-insns-single=500 -fno-rtti -fno-exceptions -Dprintf=iprintf -mcpu=cortex-m3 -DF_CPU=84000000L -DARDUINO=155-r2 -DARDUINO_SAM_DUE -DARDUINO_ARCH_SAM -D__SAM3X8E__ -mthumb -DUSB_VID=0x2341 -DUSB_PID=0x003e -DUSBCON '-DUSB_MANUFACTURER="Unknown"' '-DUSB_PRODUCT="Arduino Due"' "-IC:/AndroXStudio/legacy/arduino/hardware/arduino/sam/system/libsam" "-IC:/AndroXStudio/legacy/arduino/hardware/arduino/sam/system/CMSIS/CMSIS/Include/" "-IC:/AndroXStudio/legacy/arduino/hardware/arduino/sam/system/CMSIS/Device/ATMEL/"   -I"C:\AndroXStudio\legacy\arduino\hardware\arduino\sam\cores\arduino" -I"C:\AndroXStudio\legacy\arduino\hardware\arduino\sam\variants\arduino_due_x" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -D__IN_ECLIPSE__=1 -x c++ "$<"  -o  "$@"   -w
	@echo 'Finished building: $<'
	@echo ' '

TEXT_LCD.cpp.o: ../TEXT_LCD.cpp
	@echo 'Building file: $<'
	@echo 'Starting C++ compile'
	"C:/AndroXStudio/legacy/arduino/hardware/tools/g++_arm_none_eabi/bin/arm-none-eabi-g++" -c -g -Os -ffunction-sections -fdata-sections -nostdlib --param max-inline-insns-single=500 -fno-rtti -fno-exceptions -Dprintf=iprintf -mcpu=cortex-m3 -DF_CPU=84000000L -DARDUINO=155-r2 -DARDUINO_SAM_DUE -DARDUINO_ARCH_SAM -D__SAM3X8E__ -mthumb -DUSB_VID=0x2341 -DUSB_PID=0x003e -DUSBCON '-DUSB_MANUFACTURER="Unknown"' '-DUSB_PRODUCT="Arduino Due"' "-IC:/AndroXStudio/legacy/arduino/hardware/arduino/sam/system/libsam" "-IC:/AndroXStudio/legacy/arduino/hardware/arduino/sam/system/CMSIS/CMSIS/Include/" "-IC:/AndroXStudio/legacy/arduino/hardware/arduino/sam/system/CMSIS/Device/ATMEL/"   -I"C:\AndroXStudio\legacy\arduino\hardware\arduino\sam\cores\arduino" -I"C:\AndroXStudio\legacy\arduino\hardware\arduino\sam\variants\arduino_due_x" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -D__IN_ECLIPSE__=1 -x c++ "$<"  -o  "$@"   -w
	@echo 'Finished building: $<'
	@echo ' '



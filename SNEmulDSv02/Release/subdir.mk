################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../SNES_global.c \
../core.c \
../engine.c \
../fs.c \
../gui.c \
../main.c \
../memmap.c \
../opcodes.c \
../ppu.c \
../snapshot.c \
../snes.c 

OBJS += \
./SNES_global.o \
./core.o \
./engine.o \
./fs.o \
./gui.o \
./main.o \
./memmap.o \
./opcodes.o \
./ppu.o \
./snapshot.o \
./snes.o 

C_DEPS += \
./SNES_global.d \
./core.d \
./engine.d \
./fs.d \
./gui.d \
./main.d \
./memmap.d \
./opcodes.d \
./ppu.d \
./snapshot.d \
./snes.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	$(DEVKITARM2007)/bin/arm-eabi-gcc -DNDS -DASM_OPCODES -DARM9 -I"D:\devkitpro2007\libnds\include" -I"D:\devkitpro2007\libfat\include" -I"D:\devkitpro2007\devkitARM\arm-eabi\include" -O2 -Wall -c -fmessage-length=0 -fsigned-char -ffast-math -fomit-frame-pointer -marm -mcpu=arm946e-s -mtune=arm946e-s -DARM9 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


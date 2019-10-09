################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/sac-cli.c 

OBJS += \
./src/sac-cli.o 

C_DEPS += \
./src/sac-cli.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I/home/utnso/tp-2019-2c-SOcorro/libreriaComun/Debug -I"/home/utnso/tp-2019-2c-SOcorro/libreriaComun" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<" -DFUSE_USE_VERSION=27 -D_FILE_OFFSET_BITS=64
	@echo 'Finished building: $<'
	@echo ' '



################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../libreriaComun/libreriaComun.c 

OBJS += \
./libreriaComun/libreriaComun.o 

C_DEPS += \
./libreriaComun/libreriaComun.d 


# Each subdirectory must supply rules for building sources it contributes
libreriaComun/%.o: ../libreriaComun/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -fPIC -pthread -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



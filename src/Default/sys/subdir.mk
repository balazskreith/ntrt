################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../sys/sys_confs.c \
../sys/sys_ctrls.c \
../sys/sys_prints.c 

OBJS += \
./sys/sys_confs.o \
./sys/sys_ctrls.o \
./sys/sys_prints.o 

C_DEPS += \
./sys/sys_confs.d \
./sys/sys_ctrls.d \
./sys/sys_prints.d 


# Each subdirectory must supply rules for building sources it contributes
sys/%.o: ../sys/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/balazs/workspace/mpt024/cmp" -I"/home/balazs/workspace/mpt024/dmap" -I"/home/balazs/workspace/mpt024/etc" -I"/home/balazs/workspace/mpt024/fsm" -I"/home/balazs/workspace/mpt024/lib" -I"/home/balazs/workspace/mpt024/sys" -I"/home/balazs/workspace/mpt024/inc" -I/usr/local -O2 -g -Wall -c -fmessage-length=0 -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



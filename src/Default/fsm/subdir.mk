################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../fsm/fsm.c \
../fsm/fsm_actions.c \
../fsm/fsm_exist.c \
../fsm/fsm_halt.c \
../fsm/fsm_run.c 

OBJS += \
./fsm/fsm.o \
./fsm/fsm_actions.o \
./fsm/fsm_exist.o \
./fsm/fsm_halt.o \
./fsm/fsm_run.o 

C_DEPS += \
./fsm/fsm.d \
./fsm/fsm_actions.d \
./fsm/fsm_exist.d \
./fsm/fsm_halt.d \
./fsm/fsm_run.d 


# Each subdirectory must supply rules for building sources it contributes
fsm/%.o: ../fsm/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/balazs/workspace/mpt024/cmp" -I"/home/balazs/workspace/mpt024/dmap" -I"/home/balazs/workspace/mpt024/etc" -I"/home/balazs/workspace/mpt024/fsm" -I"/home/balazs/workspace/mpt024/lib" -I"/home/balazs/workspace/mpt024/sys" -I"/home/balazs/workspace/mpt024/inc" -I/usr/local -O2 -g -Wall -c -fmessage-length=0 -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



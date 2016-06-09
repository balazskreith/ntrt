################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../dmap/dmap.c \
../dmap/dmap_sel.c 

OBJS += \
./dmap/dmap.o \
./dmap/dmap_sel.o 

C_DEPS += \
./dmap/dmap.d \
./dmap/dmap_sel.d 


# Each subdirectory must supply rules for building sources it contributes
dmap/%.o: ../dmap/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/balazs/workspace/mpt024/cmp" -I"/home/balazs/workspace/mpt024/dmap" -I"/home/balazs/workspace/mpt024/etc" -I"/home/balazs/workspace/mpt024/fsm" -I"/home/balazs/workspace/mpt024/lib" -I"/home/balazs/workspace/mpt024/sys" -I"/home/balazs/workspace/mpt024/inc" -I/usr/local -O2 -g -Wall -c -fmessage-length=0 -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



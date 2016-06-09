################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../lib/lib_debuglog.c \
../lib/lib_dispers.c \
../lib/lib_funcs.c \
../lib/lib_heap.c \
../lib/lib_interrupting.c \
../lib/lib_makers.c \
../lib/lib_predefs.c \
../lib/lib_puffers.c \
../lib/lib_threading.c \
../lib/lib_tors.c \
../lib/lib_vector.c 

OBJS += \
./lib/lib_debuglog.o \
./lib/lib_dispers.o \
./lib/lib_funcs.o \
./lib/lib_heap.o \
./lib/lib_interrupting.o \
./lib/lib_makers.o \
./lib/lib_predefs.o \
./lib/lib_puffers.o \
./lib/lib_threading.o \
./lib/lib_tors.o \
./lib/lib_vector.o 

C_DEPS += \
./lib/lib_debuglog.d \
./lib/lib_dispers.d \
./lib/lib_funcs.d \
./lib/lib_heap.d \
./lib/lib_interrupting.d \
./lib/lib_makers.d \
./lib/lib_predefs.d \
./lib/lib_puffers.d \
./lib/lib_threading.d \
./lib/lib_tors.d \
./lib/lib_vector.d 


# Each subdirectory must supply rules for building sources it contributes
lib/%.o: ../lib/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/balazs/workspace/mpt024/cmp" -I"/home/balazs/workspace/mpt024/dmap" -I"/home/balazs/workspace/mpt024/etc" -I"/home/balazs/workspace/mpt024/fsm" -I"/home/balazs/workspace/mpt024/lib" -I"/home/balazs/workspace/mpt024/sys" -I"/home/balazs/workspace/mpt024/inc" -I/usr/local -O2 -g -Wall -c -fmessage-length=0 -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



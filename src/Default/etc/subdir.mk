################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../etc/etc_auth.c \
../etc/etc_cli.c \
../etc/etc_dict.c \
../etc/etc_endian.c \
../etc/etc_iniparser.c \
../etc/etc_ipsearch.c \
../etc/etc_utils.c 

OBJS += \
./etc/etc_auth.o \
./etc/etc_cli.o \
./etc/etc_dict.o \
./etc/etc_endian.o \
./etc/etc_iniparser.o \
./etc/etc_ipsearch.o \
./etc/etc_utils.o 

C_DEPS += \
./etc/etc_auth.d \
./etc/etc_cli.d \
./etc/etc_dict.d \
./etc/etc_endian.d \
./etc/etc_iniparser.d \
./etc/etc_ipsearch.d \
./etc/etc_utils.d 


# Each subdirectory must supply rules for building sources it contributes
etc/%.o: ../etc/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/balazs/workspace/mpt024/cmp" -I"/home/balazs/workspace/mpt024/dmap" -I"/home/balazs/workspace/mpt024/etc" -I"/home/balazs/workspace/mpt024/fsm" -I"/home/balazs/workspace/mpt024/lib" -I"/home/balazs/workspace/mpt024/sys" -I"/home/balazs/workspace/mpt024/inc" -I/usr/local -O2 -g -Wall -c -fmessage-length=0 -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



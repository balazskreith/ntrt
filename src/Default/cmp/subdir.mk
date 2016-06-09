################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../cmp/cmp_cli.c \
../cmp/cmp_cmdexetor.c \
../cmp/cmp_conasr.c \
../cmp/cmp_defs.c \
../cmp/cmp_fpsqrecver.c \
../cmp/cmp_fpsqsender.c \
../cmp/cmp_handshaker.c \
../cmp/cmp_precver.c \
../cmp/cmp_predefs.c \
../cmp/cmp_psender.c \
../cmp/cmp_pthsch.c \
../cmp/cmp_screener.c \
../cmp/cmp_tests.c \
../cmp/cmp_tunreader.c \
../cmp/cmp_tunwriter.c 

OBJS += \
./cmp/cmp_cli.o \
./cmp/cmp_cmdexetor.o \
./cmp/cmp_conasr.o \
./cmp/cmp_defs.o \
./cmp/cmp_fpsqrecver.o \
./cmp/cmp_fpsqsender.o \
./cmp/cmp_handshaker.o \
./cmp/cmp_precver.o \
./cmp/cmp_predefs.o \
./cmp/cmp_psender.o \
./cmp/cmp_pthsch.o \
./cmp/cmp_screener.o \
./cmp/cmp_tests.o \
./cmp/cmp_tunreader.o \
./cmp/cmp_tunwriter.o 

C_DEPS += \
./cmp/cmp_cli.d \
./cmp/cmp_cmdexetor.d \
./cmp/cmp_conasr.d \
./cmp/cmp_defs.d \
./cmp/cmp_fpsqrecver.d \
./cmp/cmp_fpsqsender.d \
./cmp/cmp_handshaker.d \
./cmp/cmp_precver.d \
./cmp/cmp_predefs.d \
./cmp/cmp_psender.d \
./cmp/cmp_pthsch.d \
./cmp/cmp_screener.d \
./cmp/cmp_tests.d \
./cmp/cmp_tunreader.d \
./cmp/cmp_tunwriter.d 


# Each subdirectory must supply rules for building sources it contributes
cmp/%.o: ../cmp/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/balazs/workspace/mpt024/cmp" -I"/home/balazs/workspace/mpt024/dmap" -I"/home/balazs/workspace/mpt024/etc" -I"/home/balazs/workspace/mpt024/fsm" -I"/home/balazs/workspace/mpt024/lib" -I"/home/balazs/workspace/mpt024/sys" -I"/home/balazs/workspace/mpt024/inc" -I/usr/local -O2 -g -Wall -c -fmessage-length=0 -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



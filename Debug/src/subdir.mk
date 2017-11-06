################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/bench.c \
../src/bsparseconv.c \
../src/crypto_hash_sha512.c \
../src/crypto_stream.c \
../src/fastrandombytes.c \
../src/formatc.c \
../src/hash.c \
../src/key.c \
../src/ntt.c \
../src/poly.c \
../src/sign.c \
../src/verify.c \
../src/rng.c \
../src/api.c  

OBJS += \
./src/bench.o \
./src/bsparseconv.o \
./src/crypto_hash_sha512.o \
./src/crypto_stream.o \
./src/fastrandombytes.o \
./src/formatc.o \
./src/hash.o \
./src/key.o \
./src/ntt.o \
./src/poly.o \
./src/sign.o \
./src/verify.o \
./src/rng.o \
./src/api.o

C_DEPS += \
./src/bench.d \
./src/bsparseconv.d \
./src/crypto_hash_sha512.d \
./src/crypto_stream.d \
./src/fastrandombytes.d \
./src/formatc.d \
./src/hash.d \
./src/key.d \
./src/ntt.d \
./src/poly.d \
./src/sign.d \
./src/verify.d \
./src/rng.d \
./src/api.d


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



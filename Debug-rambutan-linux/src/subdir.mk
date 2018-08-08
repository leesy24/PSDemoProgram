################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/ErrorID.c \
../src/KbhitGetch.c 

CPP_SRCS += \
../src/CRC32.cpp \
../src/CommandBase.cpp \
../src/CommandFileStream.cpp \
../src/GPINCommand.cpp \
../src/GPRMCommand.cpp \
../src/GSC2Command.cpp \
../src/GSCNCommand.cpp \
../src/GVERCommand.cpp \
../src/LinuxClientSocket.cpp \
../src/LinuxClientUART.cpp \
../src/PSDemoProgram.cpp \
../src/ProgramVersion.cpp \
../src/SCANCommand.cpp \
../src/SPRMCommand.cpp \
../src/Scan2Print.cpp \
../src/Scan2Sequence.cpp \
../src/ScanPrint.cpp \
../src/ScanSequence.cpp \
../src/WinClientSocket.cpp \
../src/WinClientUART.cpp 

OBJS += \
./src/CRC32.o \
./src/CommandBase.o \
./src/CommandFileStream.o \
./src/ErrorID.o \
./src/GPINCommand.o \
./src/GPRMCommand.o \
./src/GSC2Command.o \
./src/GSCNCommand.o \
./src/GVERCommand.o \
./src/KbhitGetch.o \
./src/LinuxClientSocket.o \
./src/LinuxClientUART.o \
./src/PSDemoProgram.o \
./src/ProgramVersion.o \
./src/SCANCommand.o \
./src/SPRMCommand.o \
./src/Scan2Print.o \
./src/Scan2Sequence.o \
./src/ScanPrint.o \
./src/ScanSequence.o \
./src/WinClientSocket.o \
./src/WinClientUART.o 

C_DEPS += \
./src/ErrorID.d \
./src/KbhitGetch.d 

CPP_DEPS += \
./src/CRC32.d \
./src/CommandBase.d \
./src/CommandFileStream.d \
./src/GPINCommand.d \
./src/GPRMCommand.d \
./src/GSC2Command.d \
./src/GSCNCommand.d \
./src/GVERCommand.d \
./src/LinuxClientSocket.d \
./src/LinuxClientUART.d \
./src/PSDemoProgram.d \
./src/ProgramVersion.d \
./src/SCANCommand.d \
./src/SPRMCommand.d \
./src/Scan2Print.d \
./src/Scan2Sequence.d \
./src/ScanPrint.d \
./src/ScanSequence.d \
./src/WinClientSocket.d \
./src/WinClientUART.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	mips-openwrt-linux-g++ -O0 -g3 -Wall -c -fmessage-length=0 -std=c++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	mips-openwrt-linux-gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



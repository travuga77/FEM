################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../source/FEM_steering.cpp 

ASM_SRCS += \
../source/DSP2833x_ADC_cal.asm \
../source/DSP2833x_CodeStartBranch.asm \
../source/DSP2833x_DisInt.asm \
../source/DSP2833x_usDelay.asm 

C_SRCS += \
../source/DSP2833x_Adc.c \
../source/DSP2833x_CpuTimers.c \
../source/DSP2833x_DefaultIsr.c \
../source/DSP2833x_ECan.c \
../source/DSP2833x_ECap.c \
../source/DSP2833x_GlobalVariableDefs.c \
../source/DSP2833x_Gpio.c \
../source/DSP2833x_MemCopy.c \
../source/DSP2833x_PieCtrl.c \
../source/DSP2833x_PieVect.c \
../source/DSP2833x_Spi.c \
../source/DSP2833x_SysCtrl.c \
../source/DSP2833x_Xintf.c \
../source/FEM_differential.c \
../source/FEM_pedal.c \
../source/FEM_safety.c \
../source/FEM_send_CAN.c \
../source/FEM_speed.c \
../source/FEM_traction.c 

C_DEPS += \
./source/DSP2833x_Adc.d \
./source/DSP2833x_CpuTimers.d \
./source/DSP2833x_DefaultIsr.d \
./source/DSP2833x_ECan.d \
./source/DSP2833x_ECap.d \
./source/DSP2833x_GlobalVariableDefs.d \
./source/DSP2833x_Gpio.d \
./source/DSP2833x_MemCopy.d \
./source/DSP2833x_PieCtrl.d \
./source/DSP2833x_PieVect.d \
./source/DSP2833x_Spi.d \
./source/DSP2833x_SysCtrl.d \
./source/DSP2833x_Xintf.d \
./source/FEM_differential.d \
./source/FEM_pedal.d \
./source/FEM_safety.d \
./source/FEM_send_CAN.d \
./source/FEM_speed.d \
./source/FEM_traction.d 

OBJS += \
./source/DSP2833x_ADC_cal.obj \
./source/DSP2833x_Adc.obj \
./source/DSP2833x_CodeStartBranch.obj \
./source/DSP2833x_CpuTimers.obj \
./source/DSP2833x_DefaultIsr.obj \
./source/DSP2833x_DisInt.obj \
./source/DSP2833x_ECan.obj \
./source/DSP2833x_ECap.obj \
./source/DSP2833x_GlobalVariableDefs.obj \
./source/DSP2833x_Gpio.obj \
./source/DSP2833x_MemCopy.obj \
./source/DSP2833x_PieCtrl.obj \
./source/DSP2833x_PieVect.obj \
./source/DSP2833x_Spi.obj \
./source/DSP2833x_SysCtrl.obj \
./source/DSP2833x_Xintf.obj \
./source/DSP2833x_usDelay.obj \
./source/FEM_differential.obj \
./source/FEM_pedal.obj \
./source/FEM_safety.obj \
./source/FEM_send_CAN.obj \
./source/FEM_speed.obj \
./source/FEM_steering.obj \
./source/FEM_traction.obj 

ASM_DEPS += \
./source/DSP2833x_ADC_cal.d \
./source/DSP2833x_CodeStartBranch.d \
./source/DSP2833x_DisInt.d \
./source/DSP2833x_usDelay.d 

CPP_DEPS += \
./source/FEM_steering.d 

OBJS__QUOTED += \
"source\DSP2833x_ADC_cal.obj" \
"source\DSP2833x_Adc.obj" \
"source\DSP2833x_CodeStartBranch.obj" \
"source\DSP2833x_CpuTimers.obj" \
"source\DSP2833x_DefaultIsr.obj" \
"source\DSP2833x_DisInt.obj" \
"source\DSP2833x_ECan.obj" \
"source\DSP2833x_ECap.obj" \
"source\DSP2833x_GlobalVariableDefs.obj" \
"source\DSP2833x_Gpio.obj" \
"source\DSP2833x_MemCopy.obj" \
"source\DSP2833x_PieCtrl.obj" \
"source\DSP2833x_PieVect.obj" \
"source\DSP2833x_Spi.obj" \
"source\DSP2833x_SysCtrl.obj" \
"source\DSP2833x_Xintf.obj" \
"source\DSP2833x_usDelay.obj" \
"source\FEM_differential.obj" \
"source\FEM_pedal.obj" \
"source\FEM_safety.obj" \
"source\FEM_send_CAN.obj" \
"source\FEM_speed.obj" \
"source\FEM_steering.obj" \
"source\FEM_traction.obj" 

C_DEPS__QUOTED += \
"source\DSP2833x_Adc.d" \
"source\DSP2833x_CpuTimers.d" \
"source\DSP2833x_DefaultIsr.d" \
"source\DSP2833x_ECan.d" \
"source\DSP2833x_ECap.d" \
"source\DSP2833x_GlobalVariableDefs.d" \
"source\DSP2833x_Gpio.d" \
"source\DSP2833x_MemCopy.d" \
"source\DSP2833x_PieCtrl.d" \
"source\DSP2833x_PieVect.d" \
"source\DSP2833x_Spi.d" \
"source\DSP2833x_SysCtrl.d" \
"source\DSP2833x_Xintf.d" \
"source\FEM_differential.d" \
"source\FEM_pedal.d" \
"source\FEM_safety.d" \
"source\FEM_send_CAN.d" \
"source\FEM_speed.d" \
"source\FEM_traction.d" 

CPP_DEPS__QUOTED += \
"source\FEM_steering.d" 

ASM_DEPS__QUOTED += \
"source\DSP2833x_ADC_cal.d" \
"source\DSP2833x_CodeStartBranch.d" \
"source\DSP2833x_DisInt.d" \
"source\DSP2833x_usDelay.d" 

ASM_SRCS__QUOTED += \
"../source/DSP2833x_ADC_cal.asm" \
"../source/DSP2833x_CodeStartBranch.asm" \
"../source/DSP2833x_DisInt.asm" \
"../source/DSP2833x_usDelay.asm" 

C_SRCS__QUOTED += \
"../source/DSP2833x_Adc.c" \
"../source/DSP2833x_CpuTimers.c" \
"../source/DSP2833x_DefaultIsr.c" \
"../source/DSP2833x_ECan.c" \
"../source/DSP2833x_ECap.c" \
"../source/DSP2833x_GlobalVariableDefs.c" \
"../source/DSP2833x_Gpio.c" \
"../source/DSP2833x_MemCopy.c" \
"../source/DSP2833x_PieCtrl.c" \
"../source/DSP2833x_PieVect.c" \
"../source/DSP2833x_Spi.c" \
"../source/DSP2833x_SysCtrl.c" \
"../source/DSP2833x_Xintf.c" \
"../source/FEM_differential.c" \
"../source/FEM_pedal.c" \
"../source/FEM_safety.c" \
"../source/FEM_send_CAN.c" \
"../source/FEM_speed.c" \
"../source/FEM_traction.c" 

CPP_SRCS__QUOTED += \
"../source/FEM_steering.cpp" 



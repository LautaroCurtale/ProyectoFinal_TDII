################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../source/ProyectoFinal_TDII.c \
../source/bluetooth.c \
../source/display_rtc_i2c.c \
../source/funciones.c \
../source/mtb.c \
../source/semihost_hardfault.c 

OBJS += \
./source/ProyectoFinal_TDII.o \
./source/bluetooth.o \
./source/display_rtc_i2c.o \
./source/funciones.o \
./source/mtb.o \
./source/semihost_hardfault.o 

C_DEPS += \
./source/ProyectoFinal_TDII.d \
./source/bluetooth.d \
./source/display_rtc_i2c.d \
./source/funciones.d \
./source/mtb.d \
./source/semihost_hardfault.d 


# Each subdirectory must supply rules for building sources it contributes
source/%.o: ../source/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DCPU_LPC845M301JBD48 -DCPU_LPC845M301JBD48_cm0plus -DFSL_RTOS_BM -DSDK_OS_BAREMETAL -DSDK_DEBUGCONSOLE=0 -DCR_INTEGER_PRINTF -DPRINTF_FLOAT_ENABLE=0 -D__MCUXPRESSO -D__USE_CMSIS -DDEBUG -D__REDLIB__ -I"C:\Users\usuario\Documents\MCUXpressoIDE_11.1.1_3241\workspace\ProyectoFinal_TDII\board" -I"C:\Users\usuario\Documents\MCUXpressoIDE_11.1.1_3241\workspace\ProyectoFinal_TDII\source" -I"C:\Users\usuario\Documents\MCUXpressoIDE_11.1.1_3241\workspace\ProyectoFinal_TDII" -I"C:\Users\usuario\Documents\MCUXpressoIDE_11.1.1_3241\workspace\ProyectoFinal_TDII\drivers" -I"C:\Users\usuario\Documents\MCUXpressoIDE_11.1.1_3241\workspace\ProyectoFinal_TDII\device" -I"C:\Users\usuario\Documents\MCUXpressoIDE_11.1.1_3241\workspace\ProyectoFinal_TDII\CMSIS" -I"C:\Users\usuario\Documents\MCUXpressoIDE_11.1.1_3241\workspace\ProyectoFinal_TDII\component\uart" -I"C:\Users\usuario\Documents\MCUXpressoIDE_11.1.1_3241\workspace\ProyectoFinal_TDII\utilities" -O0 -fno-common -g3 -Wall -c -ffunction-sections -fdata-sections -ffreestanding -fno-builtin -fmerge-constants -fmacro-prefix-map="../$(@D)/"=. -mcpu=cortex-m0plus -mthumb -D__REDLIB__ -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



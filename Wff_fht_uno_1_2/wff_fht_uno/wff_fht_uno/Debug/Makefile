################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL := cmd.exe
RM := rm -rf

USER_OBJS :=

LIBS := 
PROJ := 

O_SRCS := 
C_SRCS := 
S_SRCS := 
S_UPPER_SRCS := 
OBJ_SRCS := 
ASM_SRCS := 
PREPROCESSING_SRCS := 
OBJS := 
OBJS_AS_ARGS := 
C_DEPS := 
C_DEPS_AS_ARGS := 
EXECUTABLES := 
OUTPUT_FILE_PATH :=
OUTPUT_FILE_PATH_AS_ARGS :=
AVR_APP_PATH :=$$$AVR_APP_PATH$$$
QUOTE := "
ADDITIONAL_DEPENDENCIES:=
OUTPUT_FILE_DEP:=
LIB_DEP:=

# Every subdirectory with source files must be described here
SUBDIRS :=  \
../wff_fht/


# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS +=  \
../sampling.c \
../usart.c \
../wff_fht/complexToDecibel.c \
../wff_fht/complexToReal.c \
../wff_fht/fht.c \
../wff_fht/fhtDebug.c \
../wff_fht/generateTables.c \
../wff_fht/windowing.c \
../wff_fht_uno.c


PREPROCESSING_SRCS += 


ASM_SRCS += 


OBJS +=  \
sampling.o \
usart.o \
wff_fht/complexToDecibel.o \
wff_fht/complexToReal.o \
wff_fht/fht.o \
wff_fht/fhtDebug.o \
wff_fht/generateTables.o \
wff_fht/windowing.o \
wff_fht_uno.o

OBJS_AS_ARGS +=  \
sampling.o \
usart.o \
wff_fht/complexToDecibel.o \
wff_fht/complexToReal.o \
wff_fht/fht.o \
wff_fht/fhtDebug.o \
wff_fht/generateTables.o \
wff_fht/windowing.o \
wff_fht_uno.o

C_DEPS +=  \
sampling.d \
usart.d \
wff_fht/complexToDecibel.d \
wff_fht/complexToReal.d \
wff_fht/fht.d \
wff_fht/fhtDebug.d \
wff_fht/generateTables.d \
wff_fht/windowing.d \
wff_fht_uno.d

C_DEPS_AS_ARGS +=  \
sampling.d \
usart.d \
wff_fht/complexToDecibel.d \
wff_fht/complexToReal.d \
wff_fht/fht.d \
wff_fht/fhtDebug.d \
wff_fht/generateTables.d \
wff_fht/windowing.d \
wff_fht_uno.d

OUTPUT_FILE_PATH +=wff_fht_uno.elf

OUTPUT_FILE_PATH_AS_ARGS +=wff_fht_uno.elf

ADDITIONAL_DEPENDENCIES:=

OUTPUT_FILE_DEP:= ./makedep.mk

LIB_DEP+= 

# AVR32/GNU C Compiler



















./%.o: .././%.c
	@echo Building file: $<
	@echo Invoking: AVR/GNU C Compiler : 3.4.2
	$(QUOTE)C:\Program Files (x86)\Atmel\Atmel Toolchain\AVR8 GCC\Native\3.4.2.939\avr8-gnu-toolchain\bin\avr-gcc.exe$(QUOTE)  -funsigned-char -funsigned-bitfields -DF_CPU=16000000  -O3 -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -g2 -Wall -mmcu=atmega328p -c -std=gnu99 -MD -MP -MF "$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)"   -o "$@" "$<"
	@echo Finished building: $<
	

wff_fht/%.o: ../wff_fht/%.c
	@echo Building file: $<
	@echo Invoking: AVR/GNU C Compiler : 3.4.2
	$(QUOTE)C:\Program Files (x86)\Atmel\Atmel Toolchain\AVR8 GCC\Native\3.4.2.939\avr8-gnu-toolchain\bin\avr-gcc.exe$(QUOTE)  -funsigned-char -funsigned-bitfields -DF_CPU=16000000  -O3 -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -g2 -Wall -mmcu=atmega328p -c -std=gnu99 -MD -MP -MF "$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)"   -o "$@" "$<"
	@echo Finished building: $<
	



# AVR32/GNU Preprocessing Assembler



# AVR32/GNU Assembler




ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: $(OUTPUT_FILE_PATH) $(ADDITIONAL_DEPENDENCIES)

$(OUTPUT_FILE_PATH): $(OBJS) $(USER_OBJS) $(OUTPUT_FILE_DEP) $(LIB_DEP)
	@echo Building target: $@
	@echo Invoking: AVR/GNU Linker : 3.4.2
	$(QUOTE)C:\Program Files (x86)\Atmel\Atmel Toolchain\AVR8 GCC\Native\3.4.2.939\avr8-gnu-toolchain\bin\avr-gcc.exe$(QUOTE) -o$(OUTPUT_FILE_PATH_AS_ARGS) $(OBJS_AS_ARGS) $(USER_OBJS) $(LIBS) -Wl,-Map="wff_fht_uno.map" -Wl,--start-group -Wl,-lm  -Wl,--end-group -Wl,--gc-sections -mmcu=atmega328p 
	@echo Finished building target: $@
	"C:\Program Files (x86)\Atmel\Atmel Toolchain\AVR8 GCC\Native\3.4.2.939\avr8-gnu-toolchain\bin\avr-objcopy.exe" -O ihex -R .eeprom -R .fuse -R .lock -R .signature  "wff_fht_uno.elf" "wff_fht_uno.hex"
	"C:\Program Files (x86)\Atmel\Atmel Toolchain\AVR8 GCC\Native\3.4.2.939\avr8-gnu-toolchain\bin\avr-objcopy.exe" -j .eeprom  --set-section-flags=.eeprom=alloc,load --change-section-lma .eeprom=0  --no-change-warnings -O ihex "wff_fht_uno.elf" "wff_fht_uno.eep" || exit 0
	"C:\Program Files (x86)\Atmel\Atmel Toolchain\AVR8 GCC\Native\3.4.2.939\avr8-gnu-toolchain\bin\avr-objdump.exe" -h -S "wff_fht_uno.elf" > "wff_fht_uno.lss"
	"C:\Program Files (x86)\Atmel\Atmel Toolchain\AVR8 GCC\Native\3.4.2.939\avr8-gnu-toolchain\bin\avr-objcopy.exe" -O srec -R .eeprom -R .fuse -R .lock -R .signature  "wff_fht_uno.elf" "wff_fht_uno.srec"
	"C:\Program Files (x86)\Atmel\Atmel Toolchain\AVR8 GCC\Native\3.4.2.939\avr8-gnu-toolchain\bin\avr-size.exe" "wff_fht_uno.elf"
	
	





# Other Targets
clean:
	-$(RM) $(OBJS_AS_ARGS) $(EXECUTABLES)  
	-$(RM) $(C_DEPS_AS_ARGS)   
	rm -rf "wff_fht_uno.elf" "wff_fht_uno.a" "wff_fht_uno.hex" "wff_fht_uno.lss" "wff_fht_uno.eep" "wff_fht_uno.map" "wff_fht_uno.srec"
	
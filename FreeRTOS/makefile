CPU=arm7tdmi

BUILD=build

CFLAGS=

GCC=arm-eabi-gcc -mcpu=$(CPU) -mtune=$(CPU)

#O3 - code speed optimizate
#Os - code size optimizate
#-i /....... - way

LD=arm-eabi-gcc -nostdlib  -Wl,-T,script.ld
#-Wl,-T,script.ld -script 
#-Wl,-Ttext = 0x10000  - entery point 

OBC=arm-eabi-objcopy

VPATH=src
RTOS_PATH=$(VPATH)/RTOS
RTOS_SOURCE_PATH=$(RTOS_PATH)/Source
PORT_UTILS=$(RTOS_SOURCE_PATH)/portable/GCC/ARM7_AT91SAM7S

OBJS= startup.o main.o tasks.o queue.o list.o heap_2.o port.o portISR.o crhook.o croutine.o memcpy.o memset.o strcat.o strlen.o strncpy.o #timer.o
#$(RTOS_SOURCE_PATH)/list.o $(RTOS_SOURCE_PATH)/tasks.o $(RTOS_SOURCE_PATH)/queue.o $(PORT_UTILS)/port.o $(PORT_UTILS)/portISR.o

all: make_folder $(BUILD)/RTOS.bin

make_folder:
	if not exist build mkdir build

clean:
	del /F /Q $(BUILD)\*.*

$(BUILD)/RTOS.bin: $(BUILD)/RTOS.elf
	$(OBC) -O binary $< $@

$(BUILD)/RTOS.elf: $(OBJS:%.o=$(BUILD)/%.o)
	$(LD) $(OBJS:%.o=$(BUILD)/%.o) -o $@

$(BUILD)/%.o: %.c
	$(GCC) $(CFLAGS) -c $< -o $@
	
$(BUILD)/%.o: %.S
	$(GCC) $(CFLAGS) -c $< -o $@

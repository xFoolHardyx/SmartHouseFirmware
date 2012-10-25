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
KERNEL=$(VPATH)/kernel
HARDWARE=$(VPATH)/hardware
STRING=$(VPATH)/string

OBJS= startup.o main.o  flash.o heap_2.o print.o $(KERNEL)/list.o $(KERNEL)/queue.o $(KERNEL)/tasks.o  $(HARDWARE)/lib_AT91SAM7S256.o $(HARDWARE)/port.o $(HARDWARE)/portISR.o ParTest.o $(STRING)/memcmp.o $(STRING)/memset.o $(STRING)/strncpy.o $(STRING)/memcpy.o system.o 

all: make_folder make_floder_kernel make_floder_hardware make_floder_string $(BUILD)/My_FreeRTOS_LED.bin

make_folder:
	if not exist build mkdir build

make_floder_kernel:
	if not exist build\SRC\kernel mkdir build\SRC\kernel

make_floder_hardware:
	if not exist build\SRC\hardware mkdir build\SRC\hardware

make_floder_string:
	if not exist build\SRC\string mkdir build\SRC\string

clean:
	del /S /Q $(BUILD)\*.*

$(BUILD)/My_FreeRTOS_LED.bin: $(BUILD)/My_FreeRTOS_LED.elf
	$(OBC) -O binary $< $@

$(BUILD)/My_FreeRTOS_LED.elf: $(OBJS:%.o=$(BUILD)/%.o)
	$(LD) $(OBJS:%.o=$(BUILD)/%.o) -lgcc -LSRC -o $@

$(BUILD)/%.o: %.c
	$(GCC) $(CFLAGS) -c $< -o $@
	
$(BUILD)/%.o: %.S
	$(GCC) $(CFLAGS) -c $< -o $@

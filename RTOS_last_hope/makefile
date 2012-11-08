CPU=arm7tdmi

BUILD=build
VPATH=SRC

CFLAGS=-ISRC/include -ISRC/hardware/include

GCC=arm-eabi-gcc -mcpu=$(CPU) -mtune=$(CPU)  

#O3 - code speed optimizate
#Os - code size optimizate
#-i /....... - way

LD=arm-eabi-gcc -nostdlib  -Wl,-T,script.ld  
#-Wl,-T,script.ld -script 
#-Wl,-Ttext = 0x10000  - entery point 

OBC=arm-eabi-objcopy


KERNEL=kernel
STRING=string
HARDWARE=hardware

OBJS=	start.o \
		main.o \
		$(KERNEL)/tasks.o \
		$(KERNEL)/list.o \
		$(KERNEL)/queue.o \
		$(KERNEL)/heap_2.o \
		$(STRING)/memcmp.o \
		$(STRING)/memcpy.o \
		$(STRING)/memset.o \
		$(STRING)/strcat.o \
		$(STRING)/strlen.o \
		$(STRING)/strncpy.o \
		$(HARDWARE)/portISR.o \
		$(HARDWARE)/port.o \
		settings.o

all: make_folder $(BUILD)/FIRMWARE.bin

make_folder:
	if not exist build mkdir build
	
clean:
	del /S /Q $(BUILD)\*.*

$(BUILD)/FIRMWARE.bin: $(BUILD)/FIRMWARE.elf
	$(OBC) -O binary $< $@

$(BUILD)/FIRMWARE.elf: $(OBJS:%.o=$(BUILD)/%.o)
	$(LD) $(OBJS:%.o=$(BUILD)/%.o) -lgcc -LSRC  -o $@

$(BUILD)/%.o: %.c
	$(GCC) $(CFLAGS) -c $< -o $@
	
$(BUILD)/%.o: %.S
	$(GCC) $(CFLAGS) -c $< -o $@

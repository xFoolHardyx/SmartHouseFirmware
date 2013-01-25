CPU=arm7tdmi
BUILD=build
VPATH=SRC

CFLAGS=-ISRC/include -ISRC/hardware/include -ISRC -ISRC/string/include -ISRC/TWI/include -ISRC/TCP/include

GCC=arm-eabi-gcc -mcpu=$(CPU) -mtune=$(CPU) -g -DROM_RAN 
#-ffunction-sections -fdata-sections

#-ffunction-sections -fdata-sections - create on func and data section, on link clean 
#-g - debug inf
#O3 - code speed optimizate
#Os - code size optimizate
#-i /....... - way

LD=arm-eabi-gcc -nostdlib  -Wl,-T,script.ld


#-Wl,--gc-sections - clean info debug
#STRIP=arm-eabi-strip - debug_inf,compile_inf clean    #$(STRIP) $@ - in elf
#-Wl,-T,script.ld  
#-Wl,-Ttext = 0x10000  - entery point 

OBC=arm-eabi-objcopy

KERNEL=kernel
STRING=string
HARDWARE=hardware
TWI=TWI
TCP=TCP

OBJS=	start.o 				\
		main.o 					\
		$(KERNEL)/tasks.o 		\
		$(KERNEL)/list.o 		\
		$(KERNEL)/queue.o 		\
		$(KERNEL)/heap_2.o 		\
		$(STRING)/memcmp.o 		\
		$(STRING)/memcpy.o 		\
		$(STRING)/memset.o 		\
		$(STRING)/strcat.o 		\
		$(STRING)/strlen.o 		\
		$(STRING)/strncpy.o		\
		$(HARDWARE)/portISR.o 	\
		$(HARDWARE)/port.o 		\
		settings.o 				\
		$(TWI)/TWI.o			\
		$(TCP)/TCP.o
		
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

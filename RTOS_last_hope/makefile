CPU=arm7tdmi
BUILD=build
VPATH=SRC

CFLAGS=	-ISRC 						\
		-ISRC/include 				\
		-ISRC/hardware/include 		\
		-ISRC/string/include 		\
		-ISRC/BlockingQueue/include \
		-ISRC/dynamic/include		\
		-ISRC/PolledQueue/include	\
		-ISRC/semaphore/include		\
		-ISRC/TWI/include			\
		-ISRC/TCP/include			\
		-ISRC/HTTP/include			\
		-ISRC/other/include			
		
		
				
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
HTTP=HTTP_Server
dynamic=dynamic
PolledQueue=PolledQueue
semaphore=semaphore
BlockingQueue=BlockingQueue
HTTP=HTTP
other=other

OBJS=	start.o 					\
		main.o 						\
		$(KERNEL)/tasks.o 			\
		$(KERNEL)/list.o 			\
		$(KERNEL)/queue.o 			\
		$(KERNEL)/heap_2.o 			\
		$(STRING)/memcmp.o 			\
		$(STRING)/memcpy.o 			\
		$(STRING)/memset.o 			\
		$(STRING)/strcat.o 			\
		$(STRING)/strlen.o 			\
		$(STRING)/strncpy.o			\
		$(HARDWARE)/portISR.o 		\
		$(HARDWARE)/port.o 			\
		settings.o 					\
		$(dynamic)/dynamic.o		\
		$(PolledQueue)/PollQ.o		\
		$(semaphore)/semtest.o		\
		$(BlockingQueue)/BlockQ.o	\
		$(TWI)/TWI.o				\
		$(TWI)/TWIISR.o				\
		$(other)/lib.o				\
		$(other)/wiznet.o			\
#		$(TCP)/TCP.o				\
#		$(TCP)/TCPISR.o				\
#		$(HTTP)/HTTP_Serv.o			
		
				
all: 	make_folder_build 			\
		make_folder_hardware 		\
		make_folder_kernel 			\
		make_folder_string 			\
		make_folder_BlockingQueue 	\
		make_folder_dynamic 		\
		make_folder_PolledQueue 	\
		make_folder_semaphore 		\
		make_folder_TWI 			\
		make_folder_TCP 			\
		make_folder_HTTP			\
		make_folder_other			\
		$(BUILD)/FIRMWARE.bin

make_folder_build: 
			if not exist build mkdir build
make_folder_hardware: 
			if not exist build\hardware mkdir build\hardware 
make_folder_kernel:
			if not exist build\kernel mkdir build\kernel 
make_folder_string: 
			if not exist build\string mkdir build\string
make_folder_BlockingQueue:
			if not exist build\BlockingQueue mkdir build\BlockingQueue
make_folder_dynamic:
			if not exist build\dynamic mkdir build\dynamic
make_folder_PolledQueue:
			if not exist build\PolledQueue mkdir build\PolledQueue
make_folder_semaphore:
			if not exist build\semaphore mkdir build\semaphore
make_folder_TWI:
			if not exist build\TWI mkdir build\TWI
make_folder_TCP:
			if not exist build\TCP mkdir build\TCP
make_folder_HTTP:
			if not exist build\HTTP mkdir build\HTTP
make_folder_other:
			if not exist build\other mkdir build\other
		
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

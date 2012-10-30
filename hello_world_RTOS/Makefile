RTOS_SOURCE_DIR=FreeRTOS\Source
DEMO_COMMON_DIR=FreeRTOS\demo\Common\Minimal
DEMO_INCLUDE_DIR=FreeRTOS\demo\Common\include

CC=arm-eabi-gcc
OBJCOPY=arm-eabi-objcopy
LDSCRIPT=atmel-rom.ld

LINKER_FLAGS=-mthumb -nostartfiles -nostdlib -Xlinker -oRTOSDemo.elf -Xlinker -M -Xlinker -Map=rtosdemo.map

DEBUG=-g
OPTIM=-O3

CFLAGS=  $(OPTIM) \
		-T$(LDSCRIPT) \
		-I . \
		-I $(RTOS_SOURCE_DIR)/include \
		-I $(RTOS_SOURCE_DIR)/portable/GCC/ARM7_AT91SAM7S \
		-I $(DEMO_INCLUDE_DIR) \
		-I ./SrcAtmel \
		-I ./USB \
		-D SAM7_GCC \
		-D THUMB_INTERWORK \
		-mcpu=arm7tdmi \
		-D PACK_STRUCT_END=__attribute\(\(packed\)\) \
		-D ALIGN_STRUCT_END=__attribute\(\(aligned\(4\)\)\) \
		-fomit-frame-pointer \
		-mthumb-interwork \
		-fno-strict-aliasing \
		-fno-dwarf2-cfi-asm
										
THUMB_SOURCE= \
		main.c \
		$(RTOS_SOURCE_DIR)/list.c \
		$(RTOS_SOURCE_DIR)/queue.c \
		$(RTOS_SOURCE_DIR)/tasks.c \
		$(RTOS_SOURCE_DIR)/portable/GCC/ARM7_AT91SAM7S/port.c \
		$(RTOS_SOURCE_DIR)/portable/MemMang/heap_2.c\
		$(DEMO_COMMON_DIR)/crhook.c

ARM_SOURCE= \
		$(RTOS_SOURCE_DIR)/portable/GCC/ARM7_AT91SAM7S/portISR.c \
		./SrcAtmel/Cstartup_SAM7.c \


THUMB_OBJS = $(THUMB_SOURCE:.c=.o)
ARM_OBJS = $(ARM_SOURCE:.c=.o)


all: RTOSDemo.bin 

RTOSDemo.bin : RTOSDemo.hex
	$(OBJCOPY) RTOSDemo.elf -O binary RTOSDemo.bin
	 
RTOSDemo.hex : RTOSDemo.elf
	$(OBJCOPY) RTOSDemo.elf -O ihex RTOSDemo.hex

RTOSDemo.elf : $(THUMB_OBJS) $(ARM_OBJS) boot.s Makefile
	$(CC) $(CFLAGS) $(ARM_OBJS) $(THUMB_OBJS) $(LIBS) boot.s $(LINKER_FLAGS) 

$(THUMB_OBJS) : %.o : %.c Makefile FreeRTOSConfig.h
	$(CC) -c $(CFLAGS) -mthumb $< -o $@

$(ARM_OBJS) : %.o : %.c Makefile FreeRTOSConfig.h
	$(CC) -c $(CFLAGS) $< -o $@

clean :
	rm $(THUMB_OBJS)
	rm $(ARM_OBJS)
	touch Makefile
	rm RTOSDemo.elf
	rm RTOSDemo.hex
	
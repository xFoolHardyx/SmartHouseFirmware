CPU=arm7tdmi

BUILD=build

CFLAGS=

GCC=arm-eabi-gcc -mcpu=$(CPU) -mtune=$(CPU)

#O3 - code speed optimizate
#Os - code size optimizate

LD=arm-eabi-gcc -nostdlib -Wl,-T,script.ld
# -Wl,-T,script.ld -script 

#-Wl,-Ttext,0x10000  - entery point 

OBC=arm-eabi-objcopy

VPATH=src

OBJS=startup.o main.o 

all: make_folder $(BUILD)/IRQ_test.bin

make_folder:
	if not exist build mkdir build

clean:
	del /F /Q $(BUILD)\*.*

$(BUILD)/IRQ_test.bin: $(BUILD)/IRQ_test.elf
	$(OBC) -O binary $< $@

$(BUILD)/IRQ_test.elf: $(OBJS:%.o=$(BUILD)/%.o)
	$(LD) $(OBJS:%.o=$(BUILD)/%.o) -o $@

$(BUILD)/%.o: %.c
	$(GCC) $(CFLAGS) -c $< -o $@
	
$(BUILD)/%.o: %.S
	$(GCC) $(CFLAGS) -c $< -o $@

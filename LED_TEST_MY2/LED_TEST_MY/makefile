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

OBJS=start.o main.o system.o

all: make_folder $(BUILD)/led_test_my.bin

make_folder:
	if not exist build mkdir build

clean:
	del /F /Q $(BUILD)\*.*

$(BUILD)/led_test_my.bin: $(BUILD)/led_test_my.elf
	$(OBC) -O binary $< $@

$(BUILD)/led_test_my.elf: $(OBJS:%.o=$(BUILD)/%.o)
	$(LD) $(OBJS:%.o=$(BUILD)/%.o) -o $@

$(BUILD)/%.o: %.c
	$(GCC) $(CFLAGS) -c $< -o $@
	
$(BUILD)/%.o: %.S
	$(GCC) $(CFLAGS) -c $< -o $@

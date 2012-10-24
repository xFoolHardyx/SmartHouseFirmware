CPU=arm7tdmi

BUILD=build

CFLAGS=

GCC=arm-eabi-gcc -mcpu=$(CPU) -mtune=$(CPU) 

#O3 - code speed optimizate
#Os - code size optimizate

LD=arm-eabi-gcc -nostdlib -Wl,-T,script.ld 

#-Wl,-Ttext,0x10000  - entery point 

OBC=arm-eabi-objcopy

VPATH=src

OBJS=main_.o main.o system.o

all: make_folder $(BUILD)/_led_test.bin

make_folder:
	if not exist build mkdir build

clean:
	del /F /Q $(BUILD)\*.*

$(BUILD)/_led_test.bin: $(BUILD)/_led_test.elf
	$(OBC) -O binary $< $@

$(BUILD)/_led_test.elf: $(OBJS:%.o=$(BUILD)/%.o)
	$(LD) $(OBJS:%.o=$(BUILD)/%.o) -o $@

$(BUILD)/%.o: %.c
	$(GCC) $(CFLAGS) -c $< -o $@
	
$(BUILD)/%.o: %.S
	$(GCC) $(CFLAGS) -c $< -o $@

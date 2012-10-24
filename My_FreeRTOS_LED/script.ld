/* linker script for msm7k targets */

OUTPUT_FORMAT("elf32-littlearm", "elf32-bigarm",
	      "elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(_start)

MEMORY
{
	flash (W!RX) : ORIGIN = 0x00000000, LENGTH = 0x8000
	sram (W!RX) : ORIGIN = 0x00200000, LENGTH = 0x8000
}

__stack_top = 0x00208000;

SECTIONS
{
	.text :
	{
		. = ALIGN(4);
		__bin_start = .;
		*(.text*)
		*(.rodata*)
		. = ALIGN(4);
		__bin_end = .;
	} >flash

	.data :AT(__bin_end) 
	{
		__data_start = .;
		*(.data*)
		__data_end = .;
	} >sram

	.bss (NOLOAD) : {
		__bss_start = .;
		*(.bss)
		. = ALIGN(4);
		__bss_end = .;
	} >sram

	/DISCARD/ : { *(.note.GNU-stack) *(.gnu_debuglink) *(.comment) }
}

__end__ = .;
PROVIDE (end = .);

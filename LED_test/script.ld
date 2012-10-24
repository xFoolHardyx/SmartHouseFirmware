/* linker script for msm7k targets */

OUTPUT_FORMAT("elf32-littlearm", "elf32-bigarm",
	      "elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(_start)

MEMORY
{
	sram (W!RX) : ORIGIN = 0x00000000, LENGTH = 0x8000
}

SECTIONS
{
	.text :
	{
		. = ALIGN(4);
		__bin_start = .;
		*(.text*)
		*(.data*)
		*(.rodata*)
		. = ALIGN(4);
		__bin_end = .;
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

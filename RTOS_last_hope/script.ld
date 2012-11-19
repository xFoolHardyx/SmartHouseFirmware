/* linker script for msm7k targets */

OUTPUT_FORMAT("elf32-littlearm", "elf32-bigarm",
	      "elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(_start)

MEMORY
{
	flash (W!RX) : ORIGIN = 0x00000000, LENGTH = 0x40000
	sram (W!RX) : ORIGIN = 0x00200000, LENGTH = 0x10000
}

_stack_end = 0x20FFFC;

SECTIONS
{
	.text :
	{
		. = ALIGN(4);
		_text = .;
		*(.text*)
		*(.rodata*)
		. = ALIGN(4);
		_etext = .;
	} >flash

	.data :AT(_etext) 
	{
		_data = .;
		*(.data*)
		_edata = .;
	} >sram

	.bss (NOLOAD) : {
		_bss_start = .;
		*(.bss)
		. = ALIGN(4);
		_bss_end = .;
	} >sram

	/DISCARD/ : { *(.note.GNU-stack) *(.gnu_debuglink) *(.comment) }
}

_end = .;
PROVIDE (end = .);

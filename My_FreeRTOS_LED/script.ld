/* identify the Entry Point  (_vec_reset is defined in file crt.s)  */
ENTRY(_vec_reset)

/* specify the AT91SAM7X256 memory areas  */

MEMORY 
{
	flash 	: ORIGIN = 0, LENGTH = 128K 		/* FLASH EPROM */
	ram 	: ORIGIN = 0x200000, LENGTH = 32K   /* static RAM area */
}

/* define a global symbol _stack_end  (see analysis in annotation above) */

_stack_end = 0x207FFC;

/* now define the output sections  */

SECTIONS 
{
	. = 0; 						/* set location counter to address zero  */
	.text :						/* collect all sections that should go into FLASH after startup  */ 
	{
		*(.text) 				/* all .text sections (code)  */
		*(.rodata) 				/* all .rodata sections (constants, strings, etc.)  */
		*(.rodata*) 			/* all .rodata* sections (constants, strings, etc.)  */
		*(.glue_7) 				/* all .glue_7 sections  (no idea what these are) */
		*(.glue_7t) 			/* all .glue_7t sections (no idea what these are) */
		_etext = .; 			/* define a global symbol _etext just after the last code byte */
	} >flash 					/* put all the above into FLASH */

	
	.data : 					/* collect all initialized .data sections that go into RAM  */ 
	{
		_data = .; 				/* create a global symbol marking the start of the .data section  */
		*(.data) 				/* all .data sections  */
		_edata = .; 			/* define a global symbol marking the end of the .data section  */
	} >ram AT >flash           	/* put all the above into RAM (but load the LMA initializer copy into FLASH)  */


	.bss : 						/* collect all uninitialized .bss sections that go into RAM  */
	{
		_bss_start = .; 		/* define a global symbol marking the start of the .bss section */
		*(.bss) 				/* all .bss sections  */
	} >ram						/* put all the above in RAM (it will be cleared in the startup code */
	
	. = ALIGN(4);				/* advance location counter to the next 32-bit boundary */
	_bss_end = . ; 				/* define a global symbol marking the end of the .bss section */

}
_end = .; 						/* define a global symbol marking the end of application RAM */
	.section .text
	.code 32
	.global _start
	.type   _start, %function

_start:

	# Standard definitions of Mode bits and Interrupt (I & F) flags in PSRs

        .equ    Mode_USR,       0x10
        .equ    Mode_FIQ,       0x11
        .equ    Mode_IRQ,       0x12
        .equ    Mode_SVC,       0x13
        .equ    Mode_ABT,       0x17
        .equ    Mode_UND,       0x1B
        .equ    Mode_SYS,       0x1F

        .equ    I_Bit,          0x80    /* when I bit is set, IRQ is disabled */
        .equ    F_Bit,          0x40    /* when F bit is set, FIQ is disabled */


	# Internal Memory Base Addresses
        .equ    FLASH_BASE,     0x00100000
        .equ    RAM_BASE,       0x00200000


        .equ    Top_Stack,      0x00210000  /* AT91SAM7S256 */
        .equ    UND_Stack_Size, 0x00000004
        .equ    SVC_Stack_Size, 0x00000100
        .equ    ABT_Stack_Size, 0x00000004
        .equ    FIQ_Stack_Size, 0x00000004
        .equ    IRQ_Stack_Size, 0x00000100
        .equ    USR_Stack_Size, 0x00000400



	ldr		R0,=__stack_top

	#  Enter Undefined Instruction Mode and set its Stack Pointer
                MSR     CPSR_c, #Mode_UND|I_Bit|F_Bit
                MOV     SP, R0
                SUB     R0, R0, #UND_Stack_Size

	#  Enter Abort Mode and set its Stack Pointer
                MSR     CPSR_c, #Mode_ABT|I_Bit|F_Bit
                MOV     SP, R0
                SUB     R0, R0, #ABT_Stack_Size

	#  Enter FIQ Mode and set its Stack Pointer
                MSR     CPSR_c, #Mode_FIQ|I_Bit|F_Bit
                MOV     SP, R0
                SUB     R0, R0, #FIQ_Stack_Size

	#  Enter IRQ Mode and set its Stack Pointer
                MSR     CPSR_c, #Mode_IRQ|I_Bit|F_Bit
                MOV     SP, R0
                SUB     R0, R0, #IRQ_Stack_Size

	#	Enter Supervisor Mode and set its Stack Pointer
                MSR     CPSR_c, #Mode_SVC|I_Bit|F_Bit
                MOV     SP, R0
                SUB     R0, R0, #SVC_Stack_Size

	#  Enter System Mode and set its Stack Pointer
                MSR     CPSR_c, #Mode_SYS|I_Bit|F_Bit
                MOV     SP, R0

	#  Enter User Mode and set its Stack Pointer
	#                MSR     CPSR_c, #Mode_USR
	#                MOV     SP, R0
	#
	# Setup a default Stack Limit (when compiled with "-mapcs-stack-check")
	#                SUB     SL, SP, #USR_Stack_Size

	# We want to start in supervisor mode.  Operation will switch to system
	# mode when the first task starts.
                MSR     CPSR_c, #Mode_SVC|I_Bit|F_Bit


	ldr		r0,=__bin_end
	ldr		r1,=__data_start
	ldr		r3,=__data_end
loop1:
	cmp		r1, r3
	ldrcc	r2, [r0], #4
	strcc	r2, [r1], #4
	bcc		loop1

	ldr		r0,=__bss_start
	ldr		r1,=__bss_end
	mov		r2, #0
loop2:
	cmp		r0, r1
	strcc	r2, [r0], #4
	bcc		loop2

	bl main
	#bkpto

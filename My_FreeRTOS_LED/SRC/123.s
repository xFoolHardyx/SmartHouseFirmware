# *** Startup Code (executed after Reset) ***


# Standard definitions of Mode bits and Interrupt (I & F) flags in PSRs

        .equ    Mode_USR,       0x10
        .equ    Mode_FIQ,       0x11
        .equ    Mode_IRQ,       0x12
        .equ    Mode_SVC,       0x13
        .equ    Mode_ABT,       0x17
        .equ    Mode_UND,       0x1B
        .equ    Mode_SYS,       0x1F

        .equ    I_Bit,          0x80
        # when I bit is set, IRQ is disabled
        .equ    F_Bit,          0x40
        # when F bit is set, FIQ is disabled


# Internal Memory Base Addresses
        .equ    FLASH_BASE,     0x00100000
        .equ    RAM_BASE,       0x00200000

        .equ    Top_Stack,      0x00210000
        .equ    UND_Stack_Size, 0x00000004
        .equ    SVC_Stack_Size, 0x00000100
        .equ    ABT_Stack_Size, 0x00000004
        .equ    FIQ_Stack_Size, 0x00000004
        .equ    IRQ_Stack_Size, 0x00000100
        .equ    USR_Stack_Size, 0x00000400


# Embedded Flash Controller (EFC) definitions
        .equ    EFC_BASE,       0xFFFFFF00
        .equ    EFC_FMR,        0x60

        .equ    EFC_SETUP,      1
        .equ    EFC_FMR_Val,    0x00320100


# Watchdog Timer (WDT) definitions
        .equ    WDT_BASE,       0xFFFFFD40
        .equ    WDT_MR,         0x04

        .equ    WDT_SETUP,      1
        .equ    WDT_MR_Val,     0x00008000


# Power Mangement Controller (PMC) definitions
        .equ    PMC_BASE,       0xFFFFFC00
        .equ    PMC_MOR,        0x20
        .equ    PMC_MCFR,       0x24
        .equ    PMC_PLLR,       0x2C
        .equ    PMC_MCKR,       0x30
        .equ    PMC_SR,         0x68
        .equ    PMC_MOSCEN,     (1<<0)
        .equ    PMC_OSCBYPASS,  (1<<1)
        .equ    PMC_OSCOUNT,    (0xFF<<8)
        .equ    PMC_DIV,        (0xFF<<0)
        .equ    PMC_PLLCOUNT,   (0x3F<<8)
        .equ    PMC_OUT,        (0x03<<14)
        .equ    PMC_MUL,        (0x7FF<<16)
        .equ    PMC_USBDIV,     (0x03<<28)
        .equ    PMC_CSS,        (3<<0)
        .equ    PMC_PRES,       (7<<2)
        .equ    PMC_MOSCS,      (1<<0)
        .equ    PMC_LOCK,       (1<<2)

        .equ    PMC_SETUP,      1
        .equ    PMC_MOR_Val,    0x00000601
        .equ    PMC_PLLR_Val,   0x00191C05
        .equ    PMC_MCKR_Val,   0x00000007




#if defined(VECTORS_IN_RAM) && defined(ROM_RUN)


# Exception Vectors to be placed in RAM - added by mt
# -> will be used after remapping in ROM_RUN
# -> not needed for RAM_RUN
# Mapped to Address 0 after remapping in ROM_RUN
# Absolute addressing mode must be used.
# Dummy Handlers are implemented as infinite loops which can be modified.
# VECTORS_IN_RAM defined in makefile/by commandline

			.text
			.arm
			.section .vectram, "ax"

VectorsRAM:     LDR     PC,Reset_AddrR
                LDR     PC,Undef_AddrR
                LDR     PC,SWI_AddrR
                LDR     PC,PAbt_AddrR
                LDR     PC,DAbt_AddrR
                NOP
                LDR     PC,[PC,#-0xF20]
                LDR     PC,[PC,#-0xF20]

Reset_AddrR:     .word   Reset_Handler
Undef_AddrR:     .word   Undef_HandlerR
# SWI_AddrR:       .word   SWI_HandlerR
SWI_AddrR:       .word   SWI_Handler @@R
PAbt_AddrR:      .word   PAbt_HandlerR
DAbt_AddrR:      .word   DAbt_HandlerR
#               .word   0xdeadbeef     /* Test Reserved Address */
                 .word   0
IRQ_AddrR:       .word   IRQ_HandlerR
FIQ_AddrR:       .word   FIQ_HandlerR

Undef_HandlerR:  B       Undef_HandlerR
SWI_HandlerR:    B       SWI_HandlerR
PAbt_HandlerR:   B       PAbt_HandlerR
DAbt_HandlerR:   B       DAbt_HandlerR
IRQ_HandlerR:    B       IRQ_HandlerR
FIQ_HandlerR:    B       FIQ_HandlerR

#endif
# VECTORS_IN_RAM && ROM_RUN



 #****************************************************************************
 # Exception Vectors
 #
 #  - for ROM_RUN: placed in 0x00000000
 #  - for RAM_RUN: placed at 0x00200000 (on AT91SAM7S64)
 #  -> will be used during startup before remapping with target ROM_RUN
 #  -> will be used "always" in code without remapping or with target RAM_RUN
 #  Mapped to Address relative address 0 of .text
 #  Absolute addressing mode must be used.
 #  Dummy Handlers are implemented as infinite loops which can be modified.
 #*****************************************************************************
			.text
			.arm
			.section .vectrom, "ax"

Vectors:        LDR     PC,Reset_Addr
                LDR     PC,Undef_Addr
                LDR     PC,SWI_Addr
                LDR     PC,PAbt_Addr
                LDR     PC,DAbt_Addr
                NOP
#                LDR     PC,IRQ_Addr
                LDR     PC,[PC,#-0xF20]
#                LDR     PC,FIQ_Addr
                LDR     PC,[PC,#-0xF20]

Reset_Addr:     .word   Reset_Handler
Undef_Addr:     .word   Undef_Handler
SWI_Addr:       .word   SWI_Handler
PAbt_Addr:      .word   PAbt_Handler
DAbt_Addr:      .word   DAbt_Handler
                .word   0
IRQ_Addr:       .word   IRQ_Handler
FIQ_Addr:       .word   FIQ_Handler


#******************************************************************************
# Default exception handlers
# (These are declared weak symbols so they can be redefined in user code)
#*****************************************************************************
Undef_Handler:  B       Undef_Handler
SWI_Handler:    B       SWI_Handler
PAbt_Handler:   B       PAbt_Handler
DAbt_Handler:   B       DAbt_Handler
IRQ_Handler:    B       IRQ_Handler
FIQ_Handler:    B       FIQ_Handler

.weak Undef_Handler, SWI_Handler, PAbt_Handler, DAbt_Handler, IRQ_Handler, FIQ_Handler

# Starupt Code must be linked first at Address at which it expects to run.

		.text
		.arm
		.section .init, "ax"

		.global _startup
		.func   _startup
_startup:


# Reset Handler
                LDR     pc, =Reset_Handler
Reset_Handler:

# Setup EFC
.if EFC_SETUP
                LDR     R0, =EFC_BASE
                LDR     R1, =EFC_FMR_Val
                STR     R1, [R0, #EFC_FMR]
.endif


# Setup WDT
.if WDT_SETUP
                LDR     R0, =WDT_BASE
                LDR     R1, =WDT_MR_Val
                STR     R1, [R0, #WDT_MR]
.endif


# Setup PMC
.if PMC_SETUP
                LDR     R0, =PMC_BASE

#  Setup Main Oscillator
                LDR     R1, =PMC_MOR_Val
                STR     R1, [R0, #PMC_MOR]

#  Wait until Main Oscillator is stablilized
.if (PMC_MOR_Val & PMC_MOSCEN)
MOSCS_Loop:     LDR     R2, [R0, #PMC_SR]
                ANDS    R2, R2, #PMC_MOSCS
                BEQ     MOSCS_Loop
.endif

#  Setup the PLL
.if (PMC_PLLR_Val & PMC_MUL)
                LDR     R1, =PMC_PLLR_Val
                STR     R1, [R0, #PMC_PLLR]

#  Wait until PLL is stabilized
PLL_Loop:       LDR     R2, [R0, #PMC_SR]
                ANDS    R2, R2, #PMC_LOCK
                BEQ     PLL_Loop
.endif

#  Select Clock
                LDR     R1, =PMC_MCKR_Val
                STR     R1, [R0, #PMC_MCKR]
.endif


# Setup Stack for each mode

                LDR     R0, =Top_Stack

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

#  Enter Supervisor Mode and set its Stack Pointer
                MSR     CPSR_c, #Mode_SVC|I_Bit|F_Bit
                MOV     SP, R0
                SUB     R0, R0, #SVC_Stack_Size

#  Enter System Mode and set its Stack Pointer
                MSR     CPSR_c, #Mode_SYS|I_Bit|F_Bit
                MOV     SP, R0

#//  Enter User Mode and set its Stack Pointer
#                MSR     CPSR_c, #Mode_USR
#                MOV     SP, R0
#
#// Setup a default Stack Limit (when compiled with "-mapcs-stack-check")
#               SUB     SL, SP, #USR_Stack_Size

# We want to start in supervisor mode.  Operation will switch to system
# mode when the first task starts.
                MSR     CPSR_c, #Mode_SVC|I_Bit|F_Bit


#ifdef ROM_RUN
# Relocate .data section (Copy from ROM to RAM)
                LDR     R1, =_etext
                LDR     R2, =_data
                LDR     R3, =_edata
LoopRel:        CMP     R2, R3
                LDRLO   R0, [R1], #4
                STRLO   R0, [R2], #4
                BLO     LoopRel
#endif

# Clear .bss section (Zero init)
                MOV     R0, #0
                LDR     R1, =__bss_start__
                LDR     R2, =__bss_end__
LoopZI:         CMP     R1, R2
                STRLO   R0, [R1], #4
                BLO     LoopZI


#if defined(VECTORS_IN_RAM) || defined(RAM_RUN)

#   *** Remap ***
#   ROM_RUN: exception vectors for RAM have been already copied
#     to 0x00200000 by the .data copy-loop
#   RAM_RUN: exception vectors are already placed at 0x0020000 by
#     linker settings

				.equ    MC_BASE,0xFFFFFF00
				.equ    MC_RCR, 0x00

				LDR     R0, =MC_BASE
				MOV     R1, #1
				STR     R1, [R0, #MC_RCR]
#endif
 #VECTORS_IN_RAM || RAM_RUN

# Enter the C code
				mov   r0,#0
				mov   r1,r0
				mov   r2,r0
				mov   fp,r0
				mov   r7,r0
				ldr   r10,=main
				adr   lr, __main_exit
				bx    r10

__main_exit:    B       __main_exit


				.size   _startup, . - _startup
				.endfunc

.end

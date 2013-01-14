# Stack Sizes

.set  UND_STACK_SIZE, 0x00000004
.set  ABT_STACK_SIZE, 0x00000004
.set  FIQ_STACK_SIZE, 0x00000004
.set  IRQ_STACK_SIZE, 0X00000100
.set  SVC_STACK_SIZE, 0x00000100

# Standard definitions of Mode bits and Interrupt (I & F) flags in PSRs (program status registers)

.set  ARM_MODE_USR, 0x10
.set  ARM_MODE_FIQ, 0x11
.set  ARM_MODE_IRQ, 0x12
.set  ARM_MODE_SVC, 0x13
.set  ARM_MODE_ABT, 0x17
.set  ARM_MODE_UND, 0x1B
.set  ARM_MODE_SYS, 0x1F

.set  I_BIT, 0x80
# when I bit is set, IRQ is disabled (program status registers)
.set  F_BIT, 0x40
# when F bit is set, FIQ is disabled (program status registers)


# Addresses and offsets of AIC and PIO

.set  AT91C_BASE_AIC, 	0xFFFFF000
.set  AT91C_PIOA_CODR, 	0xFFFFF434
.set  AT91C_AIC_IVR, 	0xFFFFF100
.set  AT91C_AIC_FVR, 	0xFFFFF104
.set  AIC_IVR, 			256
.set  AIC_FVR, 			260
.set  AIC_EOICR, 		304

# Embedded Flash Controller (EFC) definitions
        .equ    EFC_BASE,       0xFFFFFF00  /* EFC Base Address */
        .equ    EFC_FMR,        0x60        /* EFC_FMR Offset */

        .equ    EFC_SETUP,      1
        .equ    EFC_FMR_Val,    0x00320100


# Watchdog Timer (WDT) definitions
        .equ    WDT_BASE,       0xFFFFFD40  /* WDT Base Address */
        .equ    WDT_MR,         0x04        /* WDT_MR Offset */

		.equ    WDT_SETUP,      1
        .equ    WDT_MR_Val,     0x00008000

# Power Mangement Controller (PMC) definitions
        .equ    PMC_BASE,       0xFFFFFC00  /* PMC Base Address */
        .equ    PMC_MOR,        0x20        /* PMC_MOR Offset */
        .equ    PMC_MCFR,       0x24        /* PMC_MCFR Offset */
        .equ    PMC_PLLR,       0x2C        /* PMC_PLLR Offset */
        .equ    PMC_MCKR,       0x30        /* PMC_MCKR Offset */
        .equ    PMC_SR,         0x68        /* PMC_SR Offset */
        .equ    PMC_MOSCEN,     (1<<0)      /* Main Oscillator Enable */
        .equ    PMC_OSCBYPASS,  (1<<1)      /* Main Oscillator Bypass */
        .equ    PMC_OSCOUNT,    (0xFF<<8)   /* Main OScillator Start-up Time */
        .equ    PMC_DIV,        (0xFF<<0)   /* PLL Divider */
        .equ    PMC_PLLCOUNT,   (0x3F<<8)   /* PLL Lock Counter */
        .equ    PMC_OUT,        (0x03<<14)  /* PLL Clock Frequency Range */
        .equ    PMC_MUL,        (0x7FF<<16) /* PLL Multiplier */
        .equ    PMC_USBDIV,     (0x03<<28)  /* USB Clock Divider */
        .equ    PMC_CSS,        (3<<0)      /* Clock Source Selection */
        .equ    PMC_PRES,       (7<<2)      /* Prescaler Selection */
        .equ    PMC_MOSCS,      (1<<0)      /* Main Oscillator Stable */
        .equ    PMC_LOCK,       (1<<2)      /* PLL Lock Status */


        .equ    PMC_SETUP,      1
        .equ    PMC_MOR_Val,    0x00000601
        .equ    PMC_PLLR_Val,   0x00191C05
        .equ    PMC_MCKR_Val,   0x00000007

# identify all GLOBAL symbols

.global _vec_reset
.global _vec_undef
.global _vec_swi
.global _vec_pabt
.global _vec_dabt
.global _vec_rsv
.global _vec_irq
.global _vec_fiq
.global AT91F_Irq_Handler
.global AT91F_Fiq_Handler
.global AT91F_Default_FIQ_handler
.global AT91F_Default_IRQ_handler
.global AT91F_Spurious_handler
.global AT91F_Dabt_Handler
.global AT91F_Pabt_Handler
.global AT91F_Undef_Handler

.extern vPortYieldProcessor


# GNU assembler controls

.text
.arm
.align
# align section on 32-bit boundary

_vec_reset: 	b _init_reset
_vec_undef: 	b AT91F_Undef_Handler
#_vec_swi: 		b _vec_swi
ldr   pc, _swi
_vec_pabt:		b AT91F_Pabt_Handler
_vec_dabt: 		b AT91F_Dabt_Handler
_vec_rsv: 		nop
#_vec_irq: 		b AT91F_Irq_Handler
LDR     PC,[PC,#-0xF20]
#_vec_fiq: 		b _vec_fiq
LDR     PC,[PC,#-0xF20]


.text
.align
_init_reset:


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

	 mov r0,#0x0
	 mov r1,r0
	 mov r2,r0
	 mov r3,r0
	 mov r4,r0
	 mov r5,r0
	 mov r6,r0
	 mov r7,r0
	 mov r8,r0
	 mov r9,r0
	 mov r10,r0
	 mov r11,r0
	 mov r12,r0
	 mov r13,r0
	 mov r14,r0


# Setup a stack for each mode with interrupts initially disabled.

     ldr   r0, =_stack_end

     msr   CPSR_c, #ARM_MODE_UND|I_BIT|F_BIT
     # switch to Undefined Instruction Mode

     mov   sp, r0
     # set stack pointer for UND mode
     sub   r0, r0, #UND_STACK_SIZE
     # adjust r0 past UND stack

     msr   CPSR_c, #ARM_MODE_ABT|I_BIT|F_BIT
     # switch to Abort Mode
     mov   sp, r0
     # set stack pointer for ABT mode
     sub   r0, r0, #ABT_STACK_SIZE
     # adjust r0 past ABT stack

     msr   CPSR_c, #ARM_MODE_FIQ|I_BIT|F_BIT
     # switch to FIQ Mode
     mov   sp, r0
     # set stack pointer for FIQ mode
     sub   r0, r0, #FIQ_STACK_SIZE
     # adjust r0 past FIQ stack

     msr   CPSR_c, #ARM_MODE_IRQ|I_BIT|F_BIT
     # switch to IRQ Mode
     mov   sp, r0
     # set stack pointer for IRQ mode
     sub   r0, r0, #IRQ_STACK_SIZE
     # adjust r0 past IRQ stack

     msr   CPSR_c, #ARM_MODE_SVC|I_BIT|F_BIT
     # switch to Supervisor Mode
     mov   sp, r0
     # set stack pointer for SVC mode
     sub   r0, r0, #SVC_STACK_SIZE
     # adjust r0 past SVC stack

     msr   CPSR_c, #ARM_MODE_SYS|I_BIT|F_BIT
     # switch to System Mode
     mov   sp, r0
     # set stack pointer for SYS mode
     # we now start execution in SYSTEM mode
     # This is exactly like USER mode (same stack)
     # but SYSTEM mode has more privileges
     # copy initialized variables .data section  (Copy from ROM to RAM)

     msr     CPSR_c, #ARM_MODE_SVC|I_BIT|F_BIT


	ldr     R1, =_etext
	ldr     R2, =_data
	ldr     R3, =_edata
loop1: 	cmp     R2, R3
	ldrcc   R0, [R1], #4
	strcc   R0, [R2], #4
	bcc     loop1

# Clear uninitialized variables .bss section (Zero init)

	mov     R0, #0
	ldr     R1, =_bss_start
  	ldr     R2, =_bss_end
loop2: 	cmp     R1, R2
	strcc   R0, [R1], #4
	bcc		loop2

# Enter the C code

  b       main

AT91F_Irq_Handler:	b AT91F_Irq_Handler

_swi:   .word vPortYieldProcessor
#_swi:   .word SWI_Handler @@R

AT91F_Dabt_Handler: 		b AT91F_Dabt_Handler
AT91F_Pabt_Handler: 		b AT91F_Pabt_Handler
AT91F_Undef_Handler: 		b AT91F_Undef_Handler
AT91F_Default_FIQ_handler: 	b AT91F_Default_FIQ_handler
AT91F_Default_IRQ_handler: 	b AT91F_Default_IRQ_handler
AT91F_Spurious_handler: 	b AT91F_Spurious_handler

#SWI_Handler:    B       SWI_Handler
.end

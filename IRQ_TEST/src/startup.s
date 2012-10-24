# Stack Sizes

.set  UND_STACK_SIZE, 0x00000010
# stack for "undefined instruction" interrupts is 16 bytes
.set  ABT_STACK_SIZE, 0x00000010
# stack for "abort" interrupts is 16 bytes
.set  FIQ_STACK_SIZE, 0x00000080
# stack for "FIQ" interrupts  is 128 bytes
.set  IRQ_STACK_SIZE, 0X00000080
# stack for "IRQ" normal interrupts is 128 bytes
.set  SVC_STACK_SIZE, 0x00000080
# stack for "SVC" supervisor mode is 128 bytes


# Standard definitions of Mode bits and Interrupt (I & F) flags in PSRs (program status registers)

.set  ARM_MODE_USR, 0x10
# Normal User Mode
.set  ARM_MODE_FIQ, 0x11
# FIQ Processing Fast Interrupts Mode
.set  ARM_MODE_IRQ, 0x12
# IRQ Processing Standard Interrupts Mode
.set  ARM_MODE_SVC, 0x13
# Supervisor Processing Software Interrupts Mode
.set  ARM_MODE_ABT, 0x17
# Abort Processing memory Faults Mode
.set  ARM_MODE_UND, 0x1B
# Undefined Processing Undefined Instructions Mode
.set  ARM_MODE_SYS, 0x1F
# System Running Priviledged Operating System Tasks Mode
.set  I_BIT, 0x80
# when I bit is set, IRQ is disabled (program status registers)
.set  F_BIT, 0x40
# when F bit is set, FIQ is disabled (program status registers)


# Addresses and offsets of AIC and PIO

.set  AT91C_BASE_AIC, 0xFFFFF000
# (AIC) Base Address
.set  AT91C_PIOA_CODR, 0xFFFFF434
# (PIO) Clear Output Data Register
.set  AT91C_AIC_IVR, 0xFFFFF100
# (AIC) IRQ Interrupt Vector Register
.set  AT91C_AIC_FVR, 0xFFFFF104
# (AIC) FIQ Interrupt Vector Register
.set  AIC_IVR, 256
# IRQ Vector Register offset from base above
.set  AIC_FVR, 260
# FIQ Vector Register offset from base above
.set  AIC_EOICR, 304
# End of Interrupt Command Register


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


# GNU assembler controls
.text
# all assembler code that follows will go into .text section
.arm
# compile for 32-bit ARM instruction set
.align
# align section on 32-bit boundary

# ============================================================
#  VECTOR TABLE
#
# Must be located in FLASH at address 0x00000000
#
#      Easy to do if this file crt.s is first in the list
# for the linker step in the makefile, e.g.
#
#     $(LD) $(LFLAGS) -o main.out  crt.o main.o
#
# ============================================================
_vec_reset: b _init_reset
# RESET vector - must be at 0x00000000
_vec_undef: b AT91F_Undef_Handler
# Undefined Instruction vector
_vec_swi: b _vec_swi
# Software Interrupt vector (endless loop)
_vec_pabt: b AT91F_Pabt_Handler
# Prefetch abort vector
_vec_dabt: b AT91F_Dabt_Handler
# Data abort vector
_vec_rsv: nop
# Reserved vector
_vec_irq: b AT91F_Irq_Handler
# Interrupt Request (IRQ) vector
_vec_fiq: b _vec_fiq
# Fast interrupt request (FIQ) vector (endless loop)

# ================================================================================
#  _init_reset Handler
#
# RESET vector 0x00000000 branches to here.
#
# ARM microprocessor begins execution after RESET at address 0x00000000
# in Supervisor mode with interrupts disabled!
#
#  _init_reset handler:  creates a stack for each ARM mode.
#                        sets up a stack pointer for each ARM mode.
#                        turns off interrupts in each mode.
#       leaves CPU in SYS (System) mode.
#
#                        block copies the initializers to .data section
#       clears the .bss section to zero
#
#       branches to main( )
# ================================================================================

.text
 # all assembler code that follows will go into .text section
.align
# align section on 32-bit boundary
_init_reset:
# Setup a stack for each mode with interrupts initially disabled.
     ldr   r0, =_stack_end
     # r0 = top-of-stack

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

	ldr     R1, =_etext
	ldr     R2, =_data
	ldr     R3, =_edata
1: 	cmp     R2, R3
	ldrlo   R0, [R1], #4
	strlo   R0, [R2], #4
	blo     1b

# Clear uninitialized variables .bss section (Zero init)

	mov     R0, #0
	ldr     R1, =_bss_start
  	ldr     R2, =_bss_end
2: 	cmp     R1, R2
	strlo   R0, [R1], #4
	blo     2b

# Enter the C code

  b       main



AT91F_Irq_Handler:
	# Manage Exception Entry

	# Adjust and save LR_irq in IRQ stack
	sub lr, lr, #4
	stmfd sp!, {lr}
	# Save r0 and SPSR (need to be saved for nested interrupt)
	mrs r14, SPSR
	stmfd sp!, {r0,r14}
	# Write in the IVR to support Protect Mode
	# No effect in Normal Mode
	# De-assert the NIRQ and clear the source in Protect Mode
	ldr r14, =AT91C_BASE_AIC
	ldr r0 , [r14, #AIC_IVR]
	str r14, [r14, #AIC_IVR]
	# Enable Interrupt and Switch in Supervisor Mode
	msr CPSR_c, #ARM_MODE_SVC
	# Save scratch/used registers and LR in User Stack
	stmfd sp!, { r1-r3, r12, r14}
	# Branch to the routine pointed by the AIC_IVR
	mov r14, pc
	bx r0
	# Manage Exception Exit
	# Restore scratch/used registers and LR from User Stack
	ldmia sp!, { r1-r3, r12, r14}
	# Disable Interrupt and switch back in IRQ mode
	msr CPSR_c, #I_BIT | ARM_MODE_IRQ
	# Mark the End of Interrupt on the AIC
	ldr r14, =AT91C_BASE_AIC
	str r14, [r14, #AIC_EOICR]
	# Restore SPSR_irq and r0 from IRQ stack
	ldmia sp!, {r0,r14}
	msr SPSR_cxsf, r14
	# Restore adjusted  LR_irq from IRQ stack directly in the PC
	ldmia sp!, {pc}^
# ========================================================================
# Function:  AT91F_Dabt_Handler
#
# Entered on Data Abort exception.
#
# ========================================================================
AT91F_Dabt_Handler: 		b AT91F_Dabt_Handler
# ========================================================================
# Function:  AT91F_Pabt_Handler
#
# Entered on Prefetch Abort exception.
#
# ========================================================================
AT91F_Pabt_Handler: 		b AT91F_Pabt_Handler
# ========================================================================
# Function:  AT91F_Undef_Handler
#
# Entered on Undefined Instruction exception.
#
# ========================================================================
AT91F_Undef_Handler: 		b AT91F_Undef_Handler
AT91F_Default_FIQ_handler: 	b AT91F_Default_FIQ_handler
AT91F_Default_IRQ_handler: 	b AT91F_Default_IRQ_handler
AT91F_Spurious_handler: 	b AT91F_Spurious_handler
.end

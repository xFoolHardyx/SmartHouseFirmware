	.section .text
	.code 32
	.global _start
	.type   _start, %function

_start:



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

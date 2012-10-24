
	.section .text
	.code 32
	.global _start
	.type   _start, %function

_start:
	bl main
	nop
	#bkpto

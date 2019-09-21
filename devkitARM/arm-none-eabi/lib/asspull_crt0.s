	.section ".init"
	.global	_start
	.align
	.thumb
_start:
	mov	r0, #0	@ int argc
	mov	r1, #0	@ char	*argv[]
	bl	main
	.end

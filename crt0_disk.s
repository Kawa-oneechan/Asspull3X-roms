//Include this from your project's crt0.s but *ONLY* if it's for disk-based code.
	.align	2
initialize:
	move	#0x2700,%sr
	move.b  #0x00,0x0D000000

	lea		_etext,%a0
	lea		0x01040000,%a1
	move.l	#_sdata,%d0
	lsr.l	#1,%d0
	bra.b	2f
1:	move.w	(%a0)+,(%a1)+
2:	dbra	%d0,1b
	move.l  #0x01000000,interface

	move	#0x2000,%sr		| enable interrupts
	jsr		main
	rts
3:	bra.b	3b

	.data

	.text

//Include this from your project's crt0.s but *ONLY* if it's for disk-based code.
	.align	2
initialize:
	move	#0x2700,%sr

	lea		_etext,%a0
	lea		0x01040000,%a1
	move.l	#_sdata,%d0
	lsr.l	#1,%d0
	subq.w	#1,%d0
2:	move.w	(%a0)+,(%a1)+
	dbra	%d0,2b
	move.l  #0x01000000,interface
	jsr		main
	rts
3:	bra.b	3b

	.data

	.text

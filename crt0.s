//Include this from your project's crt0.s.

initialize:
	move    #0x2000,%sr

	lea     _stext,%a0
	lea     0x01001000,%a1
	move.l  #_sdata,%d0
	lsr.l   #1,%d0
	subq.w  #1,%d0
2:	move.w  (%a0)+,(%a1)+
	dbra    %d0,2b

	lea     0x013F0000,%a0
	movea.l %a0,%sp
	link.w  %a6,#-8
	jsr     main
3:	bra.b   3b

	.data

	.text

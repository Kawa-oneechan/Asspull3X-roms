//Include this from your project's crt0.s.
	.align	2
initialize:
	move    #0x2700,%sr
	move.b  #0x00,0x0D000000

	lea     _etext,%a0
	lea     0x01001000,%a1
	move.l  #_sdata,%d0
	lsr.l   #1,%d0
	bra.b   2f
1:	move.w  (%a0)+,(%a1)+
2:	dbra    %d0,1b

	lea     0x013F0000,%a0
	movea.l %a0,%sp
	move.l  #0x01000000,interface
	link.w  %a6,#-8

	tst.l	__CTOR_LIST__
	beq.s	5f
	lea		__CTOR_LIST__,%a2
	move.l	#1,%d2
4:	add.l	#4,%a2
	cmp.l	__CTOR_LIST__,%d2
	bgt.s 	5f
	add.l	#1,%d2
	move.al	%a2@,%a0
	jsr		%a0@
	bra.s	4b
5:

	move    #0x2000,%sr     | enable interrupts
	jsr     main
3:	bra.b   3b

	.data

	.text

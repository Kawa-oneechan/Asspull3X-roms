        .text
        .extern ExHandler
        .extern HBlankHandler
        .extern VBlankHandler

        .long   0x01000000,initialize,ExHandler,ExHandler,ExHandler,ExHandler,ExHandler,ExHandler
        .long   ExHandler,ExHandler,ExHandler,ExHandler,ExHandler,ExHandler,ExHandler,ExHandler
        .long   ExHandler,ExHandler,ExHandler,ExHandler,ExHandler,ExHandler,ExHandler,ExHandler
        .long   ExHandler,ExHandler,ExHandler,ExHandler,HBlankHandler,ExHandler,VBlankHandler,ExHandler
        .long   ExHandler,ExHandler,ExHandler,ExHandler,ExHandler,ExHandler,ExHandler,ExHandler
        .long   ExHandler,ExHandler,ExHandler,ExHandler,ExHandler,ExHandler,ExHandler,ExHandler
        .long   ExHandler,ExHandler,ExHandler,ExHandler,ExHandler,ExHandler,ExHandler,ExHandler
        .long   ExHandler,ExHandler,ExHandler,ExHandler,ExHandler,ExHandler,ExHandler,ExHandler

initialize:
         move    #0x2700,%sr              /* disable interrupts */

|        lea     0x0FF0000,%a0
|        moveq   #0,%d0
|        move.w  #0x3FFF,%d1
| 1:
|        move.l  %d0,(%a0)+
|        dbra    %d1,1b

/* Copy initialized variables from ROM to Work RAM */
        lea     _stext,%a0
        lea     0x01000000,%a1
        move.l  #_sdata,%d0
        lsr.l   #1,%d0
        subq.w  #1,%d0
2:
        move.w  (%a0)+,(%a1)+
        dbra    %d0,2b

        lea     0x01FF0000,%a0
        movea.l %a0,%sp                   /* set stack pointer to top of Work RAM */
        link.w  %a6,#-8                  /* set up initial stack frame */

|         jsr     init_hardware           /* initialize the console hardware */

|        jsr     __INIT_SECTION__        /* do all program initializers */
        jsr     main                    /* call program main() */
|        jsr     __FINI_SECTION__        /* do all program finishers */
3:
        bra.b   3b

	.align 16
.global cartStart
cartStart:
	trap #4
|	jmp.l 0x00010004
	rts

| Exception handlers

/*
exception:
        move.l  exception_vector,-(%sp)
        beq.b   1f
        rts
1:
        addq.l  #4,%sp
        rte
hblank:
        move.l  hblank_vector,-(%sp)
        beq.b   1f
        rts
1:
        addq.l  #4,%sp
        rte

vblank:
        move.l  vblank_vector,-(%sp)
        beq.b   1f
        rts
1:
        addq.l  #1,gTicks
        addq.l  #4,%sp
        rte

illegal:
        move.l  illegal_vector,-(%sp)
        beq.b   1f
        rts
1:
        addq.l  #4,%sp
        rte
*/

| put redirection vectors and gTicks at start of Work RAM

        .data

        .text

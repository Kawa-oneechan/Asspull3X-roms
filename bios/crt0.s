	.text
	.extern ExHandler
	.extern AddressHandler
	.extern InstructionHandler
	.extern ZeroHandler
	.extern nullHandler

	.long	0x01000000		|	0. Initial SSP
	.long	initialize		|	1. Initial PC
	.long	ExHandler		|	2. Bus error
	.long	AddressHandler		|	3. Address error
	.long	InstructionHandler	|	4. Illegal instruction
	.long	ZeroHandler		|	5. Division by zero
	.long	nullHandler		|	6. CHK instruction
	.long	nullHandler		|	7. TRAPV instruction
	.long	nullHandler		|	8. Check your privilege, scrublord.
	.long	nullHandler		|	9. Trace
	.long	nullHandler		| 10. Unimplemented instruction
	.long	nullHandler		| 11. Unimplemented instruction
	.long	nullHandler		| 12. 
	.long	nullHandler		| 13. Reserved by Motorola
	.long	nullHandler		| 14. 
	.long	nullHandler		| 15. Uninitialized interrupt
	.long	nullHandler		| 16. Reserved by Motorola
	.long	nullHandler		| 17. Reserved by Motorola
	.long	nullHandler		| 18. Reserved by Motorola
	.long	nullHandler		| 19. Reserved by Motorola
	.long	nullHandler		| 20. Reserved by Motorola
	.long	nullHandler		| 21. Reserved by Motorola
	.long	nullHandler		| 22. Reserved by Motorola
	.long	nullHandler		| 23. Reserved by Motorola
	.long	nullHandler		| 24. Spurious interrupt
	.long	nullHandler		| 25. Level 1 autovector
	.long	nullHandler		| 26. Level 2 autovector
	.long	nullHandler		| 27. Level 3 autovector
	.long	HBlankHandler	| 28. Level 4 autovector
	.long	nullHandler		| 29. Level 5 autovector
	.long	VBlankHandler	| 30. Level 6 autovector
	.long	nullHandler		| 31. Level 7 autovector
	.long	nullHandler		| 32. TRAP0
	.long	nullHandler		| 33. TRAP1
	.long	nullHandler		| 34. TRAP2
	.long	nullHandler		| 35. TRAP3
	.long	nullHandler		| 36. TRAP4
	.long	nullHandler		| 37. TRAP5
	.long	nullHandler		| 38. TRAP6
	.long	nullHandler		| 39. TRAP7
	.long	nullHandler		| 40. TRAP8
	.long	nullHandler		| 41. TRAP9
	.long	nullHandler		| 42. TRAP10
	.long	nullHandler		| 43. TRAP11
	.long	nullHandler		| 44. TRAP12
	.long	nullHandler		| 45. TRAP13
	.long	nullHandler		| 46. TRAP14
	.long	nullHandler		| 47. TRAP15
	.long	nullHandler		| 48. Reserved by Motorola
	.long	nullHandler		| 49. Reserved by Motorola
	.long	nullHandler		| 50. Reserved by Motorola
	.long	nullHandler		| 51. Reserved by Motorola
	.long	nullHandler		| 52. Reserved by Motorola
	.long	nullHandler		| 53. Reserved by Motorola
	.long	nullHandler		| 54. Reserved by Motorola
	.long	nullHandler		| 55. Reserved by Motorola
	.long	nullHandler		| 56. Reserved by Motorola
	.long	nullHandler		| 57. Reserved by Motorola
	.long	nullHandler		| 58. Reserved by Motorola
	.long	nullHandler		| 59. Reserved by Motorola
	.long	nullHandler		| 60. Reserved by Motorola
	.long	nullHandler		| 61. Reserved by Motorola
	.long	nullHandler		| 62. Reserved by Motorola
	.long	nullHandler		| 63. Reserved by Motorola

initialize:
	move	#0x2700,%sr		| disable interrupts

| Copy initialized variables from ROM to Work RAM
	lea		_etext,%a0
	lea		0x01000000,%a1
	move.l	#_sdata,%d0
	lsr.l	#1,%d0
	bra.b	2f
1:	move.w	(%a0)+,(%a1)+
2:	dbra	%d0,1b

	lea		0x013F0000,%a0
	movea.l	%a0,%sp			| set stack pointer to top of Work RAM
	link.w	%a6,#-8			| set up initial stack frame

	move	#0x2000,%sr		| enable interrupts
	jsr		main			| GO!
3:
	bra.b	3b

nullHandler:	rte			| Empty exception handler does nothing

.macro IRQHandler name, offset
\name:
	move.l	%d0,-(%sp)
	move.l	interface+\offset,%d0
	tst.l	%d0
	beq.b	1f
	movem.l	%d1/%a0-%a1,-(%sp)	| preserve volatile registers
	pea		(0x10,%sp)			| push exception frame pointer
	movea.l	%d0,%a0
	jsr		(%a0)
	addq.l	#4,%sp
	movem.l	(%sp)+,%d1/%a0-%a1
1:	move.l	(%sp)+,%d0
	rte
.endm

IRQHandler VBlankHandler, 0xC
IRQHandler HBlankHandler, 0x10

	.align 16

	.data
	.extern textLibrary

/* For reference:
typedef struct IBios
{
	long AssBang;
	int16_t biosVersion;
	int16_t extensions;
	void(*Exception)(void*);
	void(*VBlank)(void*);
	void(*HBlank)(void*);
	void(*DrawChar)(char, int32_t, int32_t, int32_t);
	ITextLibrary* textLibrary;
	IDrawingLibrary* drawingLibrary;
	IMiscLibrary* miscLibrary;
	IDiskLibrary* diskLibrary;
	char* DrawCharFont;
	uint16_t DrawCharHeight;
	uint8_t* LinePrinter;
}
*/
interface:
	.long 0x41535321	| ASS!
	.short 0x0003, 0	| 0.3, no extensions
	.long ExHandler
	.long 0			| No VBlank
	.long 0			| No HBlank
	.long 0			| No graphical text routine
	.long textLibrary
	.long drawingLibrary
	.long miscLibrary
	.long diskLibrary
	.long 0x0E050C00	| Bold
	.short 0x0808		| 8x8, line height 8
	.long 0x00000000	| No line printer

	.text

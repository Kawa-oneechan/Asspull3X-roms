
@{{BLOCK(pal)

@=======================================================================
@
@	pal, 128x40@4, 
@	+ palette 32 entries, not compressed
@	Total size: 64 = 64
@
@	Time-stamp: 2019-10-04, 18:46:49
@	Exported by Cearn's GBA Image Transmogrifier
@	( http://www.coranac.com )
@
@=======================================================================

	.section .rodata
	.align	2
	.global palPal		@ 64 unsigned chars
palPal:
	.hword 0x0000,0x1084,0x01A0,0x0A62,0x0340,0x2BAA,0x57F3,0x0554
	.hword 0x11D8,0x125C,0x0000,0x0000,0x0000,0x6335,0x73B9,0x7FFF
	.hword 0x0000,0x0842,0x008E,0x1575,0x1A3A,0x3F5F,0x0000,0x050D
	.hword 0x0DF3,0x1ADA,0x21C2,0x3246,0x3EA9,0x4B0C,0x534E,0x7FFF

@}}BLOCK(pal)

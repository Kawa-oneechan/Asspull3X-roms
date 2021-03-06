	.section .rodata
	.align	2
	.global tilesetPal
	.global tilesetTiles
tilesetPal:
	.short 0x0000,0x1084,0x01A0,0x0A62,0x0340,0x2BAA,0x57F3,0x0554
	.short 0x11D8,0x125C,0x0000,0x0000,0x0000,0x6335,0x73B9,0x7FFF
	.short 0x0000,0x0842,0x008E,0x1575,0x1A3A,0x3F5F,0x0000,0x050D
	.short 0x0DF3,0x1ADA,0x21C2,0x3246,0x3EA9,0x4B0C,0x534E,0x7FFF

tilesetTiles:
	.byte 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
	.byte 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
	.byte 0x70,0x77,0x77,0x77,0x77,0x9F,0x88,0x88,0xF7,0x99,0x99,0x99,0xF7,0x99,0xF9,0xFF
	.byte 0x97,0x99,0xFF,0x77,0x87,0x99,0xFF,0x97,0x87,0x99,0xFF,0x97,0x87,0x99,0x79,0xF7
	.byte 0x77,0x77,0x77,0x07,0x88,0x88,0x78,0x77,0x99,0x99,0x88,0x77,0xFF,0x99,0x89,0x77
	.byte 0xF7,0x9F,0x89,0x77,0xF9,0x7F,0x89,0x77,0xFF,0x7F,0x89,0x77,0x7F,0x77,0x89,0x77
	.byte 0x87,0x99,0x99,0xF9,0x87,0x99,0x99,0x99,0x87,0x99,0x99,0xF9,0x87,0x99,0x99,0xF9
	.byte 0x87,0x98,0x99,0x99,0x77,0x88,0x88,0x88,0x77,0x77,0x77,0x77,0x70,0x77,0x77,0x77

	.byte 0x7F,0x99,0x89,0x77,0x77,0x99,0x89,0x77,0x9F,0x99,0x89,0x77,0x7F,0x99,0x89,0x77
	.byte 0x77,0x99,0x89,0x77,0x88,0x88,0x78,0x77,0x77,0x77,0x77,0x77,0x77,0x77,0x77,0x07
	.byte 0x51,0x44,0x33,0x33,0x41,0x22,0x22,0x22,0x31,0x22,0x22,0x22,0x11,0x11,0x11,0x11
	.byte 0x33,0x33,0x51,0x44,0x22,0x22,0x41,0x22,0x22,0x22,0x31,0x22,0x11,0x11,0x11,0x11
	.byte 0x11,0x11,0x11,0x11,0x41,0x64,0xF6,0xFF,0x21,0x22,0x54,0xF6,0x21,0x43,0x55,0xF6
	.byte 0x21,0x43,0x55,0xF6,0x21,0x43,0x55,0xF6,0x21,0x43,0x55,0xF6,0x21,0x43,0x55,0xF6
	.byte 0x11,0x11,0x11,0x11,0xFF,0x6F,0x66,0x66,0x56,0x44,0x22,0x22,0x66,0x55,0x34,0x44
	.byte 0x66,0x55,0x34,0x44,0x66,0x55,0x34,0x44,0x66,0x55,0x34,0x44,0x66,0x55,0x34,0x44

	.byte 0x11,0x11,0x11,0x11,0x66,0x66,0x66,0x66,0x22,0x22,0x22,0x22,0x44,0x44,0x34,0x33
	.byte 0x44,0x44,0x34,0x33,0x44,0x44,0x34,0x33,0x44,0x44,0x34,0x33,0x44,0x44,0x34,0x33
	.byte 0x11,0x11,0x11,0x11,0x46,0x44,0x44,0x14,0x22,0x22,0x22,0x11,0x23,0x22,0x22,0x11
	.byte 0x23,0x22,0x22,0x11,0x23,0x22,0x22,0x11,0x23,0x22,0x22,0x11,0x23,0x22,0x22,0x11
	.byte 0x21,0x43,0x55,0xF6,0x21,0x43,0x55,0xF6,0x21,0x43,0x55,0xF6,0x21,0x43,0x55,0xF6
	.byte 0x21,0x43,0x55,0xF6,0x21,0x43,0x55,0xF6,0x11,0x11,0x11,0x11,0x00,0x11,0x11,0x11
	.byte 0x66,0x55,0x34,0x44,0x66,0x55,0x34,0x44,0x66,0x55,0x34,0x44,0x66,0x55,0x34,0x44
	.byte 0x66,0x55,0x34,0x44,0x66,0x55,0x34,0x44,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11

	.byte 0x44,0x44,0x34,0x33,0x44,0x44,0x34,0x33,0x44,0x44,0x34,0x33,0x44,0x44,0x34,0x33
	.byte 0x44,0x44,0x34,0x33,0x44,0x44,0x34,0x33,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11
	.byte 0x23,0x22,0x22,0x11,0x23,0x22,0x22,0x11,0x23,0x22,0x22,0x11,0x23,0x22,0x22,0x11
	.byte 0x23,0x22,0x22,0x11,0x23,0x22,0x22,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x00
	.byte 0x00,0x21,0x43,0x44,0x00,0x21,0x43,0x44,0x00,0x21,0x43,0x44,0x00,0x21,0x43,0x44
	.byte 0x00,0x21,0x43,0x44,0x00,0x21,0x43,0x44,0x00,0x21,0x43,0x44,0x00,0x21,0x43,0x44
	.byte 0xF6,0x56,0x45,0x43,0xF6,0x56,0x45,0x43,0xF6,0x56,0x45,0x43,0xF6,0x56,0x45,0x43
	.byte 0xF6,0x56,0x45,0x43,0xF6,0x56,0x45,0x43,0xF6,0x56,0x45,0x43,0xF6,0x56,0x45,0x43

	.byte 0x44,0x34,0x33,0x23,0x44,0x34,0x33,0x23,0x44,0x34,0x33,0x23,0x44,0x34,0x33,0x23
	.byte 0x44,0x34,0x33,0x23,0x44,0x34,0x33,0x23,0x44,0x34,0x33,0x23,0x44,0x34,0x33,0x23
	.byte 0x22,0x22,0x11,0x00,0x22,0x22,0x11,0x00,0x22,0x22,0x11,0x00,0x22,0x22,0x11,0x00
	.byte 0x22,0x22,0x11,0x00,0x22,0x22,0x11,0x00,0x22,0x22,0x11,0x00,0x22,0x22,0x11,0x00
	.byte 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x02,0x00,0x00,0x20,0x26,0x00
	.byte 0x00,0x62,0x26,0x20,0x00,0x52,0x62,0x62,0x00,0x42,0x25,0x55,0x22,0x26,0x25,0x45
	.byte 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x22,0x00,0x00,0x22,0x55,0x02,0x00
	.byte 0x56,0x24,0x00,0x00,0x45,0x22,0x00,0x00,0x24,0x44,0x02,0x00,0x42,0x23,0x20,0x22

	.byte 0x00,0x02,0x20,0x55,0x20,0x25,0x22,0x32,0x20,0x62,0x66,0x25,0x42,0x22,0x45,0x55
	.byte 0x24,0x52,0x22,0x54,0x22,0x26,0x25,0x43,0x22,0x25,0x54,0x32,0x24,0x44,0x32,0x34
	.byte 0x65,0x66,0x42,0x34,0x24,0x55,0x26,0x23,0x43,0x52,0x26,0x23,0x32,0x24,0x65,0x42
	.byte 0x32,0x24,0x55,0x52,0x24,0x53,0x54,0x32,0x24,0x43,0x54,0x34,0x33,0x44,0x44,0x33
	.byte 0x32,0x23,0x62,0x55,0x34,0x62,0x46,0x22,0x24,0x56,0x24,0x20,0x63,0x45,0x02,0x62
	.byte 0x54,0x34,0x23,0x56,0x45,0x34,0x54,0x45,0x43,0x33,0x43,0x33,0x33,0x43,0x33,0x33
	.byte 0x02,0x00,0x00,0x00,0x00,0x22,0x02,0x00,0x22,0x66,0x25,0x02,0x56,0x25,0x22,0x00
	.byte 0x24,0x52,0x45,0x02,0x42,0x45,0x22,0x24,0x43,0x24,0x00,0x02,0x34,0x23,0x00,0x00

	.byte 0x11,0x11,0x11,0x11,0x34,0x55,0x55,0x35,0x55,0x44,0x44,0x54,0x44,0x33,0x33,0x44
	.byte 0x33,0x33,0x33,0x33,0x13,0x33,0x13,0x33,0x13,0x14,0x13,0x13,0x13,0x15,0x14,0x13
	.byte 0x11,0x11,0x11,0x11,0x44,0x53,0x55,0x35,0x55,0x45,0x44,0x54,0x44,0x34,0x33,0x43
	.byte 0x13,0x33,0x33,0x33,0x13,0x33,0x13,0x33,0x14,0x13,0x14,0x13,0x15,0x14,0x15,0x14
	.byte 0x14,0x11,0x15,0x13,0x15,0x77,0x11,0x14,0x71,0x77,0x17,0x15,0x77,0x88,0x78,0x71
	.byte 0x89,0x88,0x88,0x77,0x89,0x99,0x89,0x77,0x88,0x99,0x89,0x88,0x88,0x99,0x89,0x88
	.byte 0x11,0x15,0x11,0x15,0x77,0x11,0x77,0x11,0x77,0x77,0x77,0x17,0x87,0x77,0x88,0x87
	.byte 0x88,0x88,0x99,0x98,0x88,0x88,0x99,0x98,0x88,0x88,0x99,0x88,0x77,0x88,0x88,0x88

	.byte 0x88,0x77,0x88,0x88,0x88,0x88,0x88,0x99,0x88,0x88,0x88,0x99,0x97,0x89,0x88,0x88
	.byte 0x97,0x89,0x88,0x88,0x98,0x89,0x78,0x87,0x88,0x88,0x78,0x87,0x88,0x77,0x88,0x88
	.byte 0x77,0x88,0x88,0x88,0x88,0x98,0x99,0x88,0x88,0x98,0x99,0x88,0x88,0x98,0x99,0x78
	.byte 0x88,0x88,0x88,0x78,0x88,0x78,0x87,0x88,0x99,0x78,0x87,0x88,0x99,0x88,0x88,0x88
	.byte 0x99,0x88,0x88,0x88,0x88,0x98,0x99,0x88,0x88,0x98,0x99,0x88,0x88,0x98,0x99,0x78
	.byte 0x88,0x88,0x88,0x78,0x88,0x98,0x89,0x88,0x99,0x98,0x89,0x88,0x99,0x88,0x88,0x88
	.byte 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xD0
	.byte 0x00,0x00,0x00,0xED,0x00,0x00,0xD0,0xFE,0x00,0x00,0xED,0xFF,0x00,0xD0,0xFE,0xFF

	.byte 0x00,0x00,0xD0,0xDD,0x00,0xDD,0xED,0xEE,0xDD,0xEE,0xFE,0xFF,0xEE,0xFF,0xFF,0xFF
	.byte 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF
	.byte 0xDD,0x0D,0x00,0x00,0xEE,0xDE,0x0D,0x00,0xFF,0xEF,0xDE,0x00,0xFF,0xFF,0xEF,0x0D
	.byte 0xFF,0xFF,0xEF,0x0D,0xFF,0xFF,0xFF,0xDE,0xFF,0xFF,0xFF,0xDE,0xFF,0xFF,0xFF,0xDE
	.byte 0x00,0xED,0xFF,0xFF,0x00,0xED,0xFF,0xFF,0xD0,0xFE,0xFF,0xFF,0xD0,0xFE,0xFF,0xFF
	.byte 0xD0,0xFE,0xFF,0xFF,0xED,0xFF,0xFF,0xFF,0xED,0xFF,0xFF,0xFF,0xED,0xFF,0xFF,0xFF
	.byte 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF
	.byte 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF

	.byte 0xFF,0xFF,0xFF,0xDE,0xFF,0xFF,0xFF,0xEF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF
	.byte 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF
	.byte 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFE,0xFF,0xFF,0xEF,0xED,0xFF,0xFF,0xEF,0xED,0xFF
	.byte 0xFF,0xEF,0xED,0xFF,0xFF,0xEF,0xED,0xFF,0xFF,0xEF,0xED,0xFF,0xFF,0xFF,0xFE,0xFF
	.byte 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xEF,0xFE,0xFF,0xFF,0xDE,0xFD,0xFF,0xFF
	.byte 0xDE,0xED,0xFE,0xFF,0xEF,0xDE,0xED,0xEE,0xFF,0xEF,0xDE,0xDD,0xFF,0xFF,0xEF,0xEE
	.byte 0xEE,0xFE,0xFF,0xFF,0xDD,0xED,0xEE,0xFF,0xEE,0xDE,0xDD,0xEE,0xFF,0xEF,0xEE,0xDD
	.byte 0xFF,0xFF,0xFF,0xEE,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF

	.byte 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFE,0xFF,0xFF,0xFF
	.byte 0xED,0xFF,0xFF,0xFF,0xDE,0xFE,0xFF,0xFF,0xEF,0xED,0xFF,0xFF,0xFF,0xDE,0xFE,0xFF
	.byte 0xFF,0xFF,0xEF,0xEE,0xFF,0xEF,0xDE,0xDD,0xFF,0xDE,0xED,0xEE,0xEF,0xDD,0xFE,0xFF
	.byte 0xEF,0xED,0xFF,0xFF,0xDE,0xFE,0xFF,0xFF,0xDE,0xFE,0xFF,0xFF,0xDE,0xFE,0xFF,0xFF
	.byte 0xFF,0xEF,0xED,0xFF,0xFF,0xEF,0xED,0xFF,0xFF,0xFF,0xDE,0xFE,0xFF,0xFF,0xDE,0xFE
	.byte 0xFF,0xFF,0xDE,0xFE,0xFF,0xFF,0xEF,0xED,0xFF,0xFF,0xEF,0xED,0xFF,0xFF,0xEF,0xED
	.byte 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
	.byte 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xE0,0x00,0x00,0xEE,0xEE,0x00,0xEE,0xEE,0xDE

	.byte 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
	.byte 0x00,0xE0,0xEE,0xEE,0xEE,0xEE,0xDE,0xDD,0xEE,0xDE,0xCD,0xCC,0xDD,0xCD,0xCC,0xCC
	.byte 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
	.byte 0xEE,0xDD,0x0D,0x00,0xCC,0xBB,0xDB,0xDD,0xCC,0xCC,0xBB,0xBB,0xCC,0xCC,0xCC,0xBB
	.byte 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
	.byte 0x00,0x00,0x00,0x00,0x0D,0x00,0x00,0x00,0xDA,0xDD,0x00,0x00,0xAB,0xAA,0xDD,0x00
	.byte 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xE0,0x00,0x00,0x00,0xEE,0x00,0x00,0xEE,0xDE
	.byte 0x00,0xE0,0xEE,0xDD,0x00,0xEE,0xDD,0xCD,0xE0,0xDE,0xCD,0xCC,0xEE,0xDD,0xCC,0xCC

	.byte 0xEE,0xEE,0xDD,0xDD,0xEE,0xDE,0xCD,0xCC,0xDE,0xDD,0xCC,0xCC,0xDD,0xCC,0xCC,0xCC
	.byte 0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC
	.byte 0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC
	.byte 0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC
	.byte 0xBB,0xBB,0xAA,0xDD,0xCC,0xBB,0xBB,0xAA,0xCC,0xBC,0xBB,0xBB,0xCC,0xCC,0xBB,0xBB
	.byte 0xCC,0xCC,0xBC,0xBB,0xCC,0xCC,0xCC,0xBB,0xCC,0xCC,0xCC,0xBC,0xCC,0xCC,0xCC,0xBC
	.byte 0x00,0x00,0x00,0x00,0xDD,0x00,0x00,0x00,0xAA,0x0D,0x00,0x00,0xBB,0xDA,0x00,0x00
	.byte 0xBB,0xAB,0x0D,0x00,0xBB,0xBB,0xDA,0x00,0xBB,0xBB,0xAB,0x0D,0xBB,0xBB,0xBB,0xDA

	.byte 0x00,0x00,0x00,0xE0,0x00,0x00,0x00,0xEE,0x00,0x00,0xE0,0xDE,0x00,0x00,0xEE,0xDD
	.byte 0x00,0xE0,0xDE,0xCD,0x00,0xEE,0xDD,0xCC,0xE0,0xDE,0xCD,0xCC,0xEE,0xDD,0xCC,0xCC
	.byte 0xDE,0xCD,0xCC,0xCC,0xDD,0xCC,0xCC,0xCC,0xCD,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC
	.byte 0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC
	.byte 0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xDD,0xDD,0xCC
	.byte 0xDC,0xEE,0xEE,0xCD,0xDC,0xEE,0xEE,0xCD,0xED,0xEE,0xEE,0xDE,0xED,0xEE,0xEE,0xDE
	.byte 0xBB,0xBB,0xBB,0xAB,0xBC,0xBB,0xBB,0xBB,0xBC,0xBB,0xBB,0xBB,0xCC,0xBB,0xBB,0xBB
	.byte 0xCC,0xBC,0xBB,0xBB,0xCC,0xBC,0xBB,0xBB,0xCC,0xCC,0xBB,0xBB,0xCC,0xCC,0xBC,0xBB

	.byte 0x0D,0x00,0x00,0x00,0xDA,0x00,0x00,0x00,0xAB,0x0D,0x00,0x00,0xBB,0xDA,0x00,0x00
	.byte 0xBB,0xAB,0x0D,0x00,0xBB,0xBB,0xDA,0x00,0xBB,0xBB,0xAB,0x0D,0xBB,0xBB,0xBB,0xDA
	.byte 0xCC,0xCC,0xBC,0xCC,0xCC,0xCC,0xAB,0xCB,0xCC,0xBC,0xAA,0xBA,0xBC,0xBC,0xAA,0xBA
	.byte 0xAB,0xBB,0xAA,0xBA,0xAB,0xBB,0xAA,0xBA,0xAB,0xCB,0xAB,0xCB,0xBC,0xCC,0xBC,0xCC
	.byte 0xCC,0xCC,0xBC,0xBB,0xCC,0xCC,0xCC,0xBB,0xCC,0xCC,0xCC,0xBC,0xCC,0xCC,0xCC,0xBC
	.byte 0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC
	.byte 0xBB,0xBB,0xBB,0xAB,0xBB,0xBB,0xBB,0xBB,0xBB,0xBB,0xBB,0xBB,0xBB,0xBB,0xBB,0xBB
	.byte 0xBB,0xBB,0xBB,0xBB,0xBB,0xBB,0xBB,0xBB,0xBC,0xBB,0xBB,0xBB,0xCC,0xBB,0xBB,0xBB

	.byte 0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xBB,0xCC,0xCC,0xBC,0xAA,0xCC,0xCC,0xAB,0xAA
	.byte 0xCC,0xCC,0xAB,0xAA,0xCC,0xBC,0xAA,0xAA,0xCC,0xBC,0xAA,0xAA,0xCC,0xBC,0xAA,0xAA
	.byte 0xCC,0xBC,0xBB,0xBB,0xCC,0xBC,0xBB,0xBB,0xCC,0xCC,0xBB,0xBB,0xCC,0xCC,0xBB,0xBB
	.byte 0xCC,0xCC,0xBC,0xBB,0xCC,0xCC,0xCC,0xBB,0xCC,0xCC,0xCC,0xBB,0xCC,0xCC,0xCC,0xBC
	.byte 0xBB,0xBB,0xBB,0xAB,0xBB,0xBB,0xBB,0xBB,0xBB,0xBB,0xBB,0xBB,0xBB,0xBB,0xBB,0xBB
	.byte 0xBB,0xBB,0xBB,0xBB,0xBB,0xBB,0xBB,0xBB,0xBB,0xBB,0xBB,0xBB,0xBB,0xBB,0xBB,0xBB
	.byte 0xCC,0xCC,0xCC,0xDE,0xCC,0xCC,0xDE,0xDD,0xCC,0xEC,0xDD,0xCC,0xCC,0xDE,0xCC,0xCC
	.byte 0xEC,0xCD,0xCC,0xCC,0xEC,0xCD,0xCC,0xCC,0xDE,0xCC,0xCC,0xCC,0xDE,0xCC,0xCC,0xCC

	.byte 0xBB,0xCB,0xCC,0xCC,0xAB,0xBA,0xCC,0xCC,0xBB,0xAB,0xCB,0xCC,0xCC,0xBB,0xBA,0xCC
	.byte 0xCC,0xCC,0xAB,0xCB,0xCC,0xCC,0xAB,0xCB,0xCC,0xCC,0xBC,0xBA,0xCC,0xCC,0xBC,0xBA
	.byte 0xBB,0xBB,0xBB,0xBB,0xBC,0xBB,0xBB,0xBB,0xBC,0xBB,0xBB,0xBB,0xCC,0xBB,0xBB,0xBB
	.byte 0xCC,0xBC,0xBB,0xBB,0xCC,0xBC,0xBB,0xBB,0xCC,0xCC,0xBB,0xBB,0xCC,0xCC,0xBC,0xBB
	.byte 0x00,0x00,0x00,0xE0,0x00,0x00,0x00,0xEE,0x00,0x00,0xE0,0xDE,0x00,0x00,0xEE,0xDD
	.byte 0xEE,0xDD,0xDD,0xCD,0xCC,0xBB,0xDB,0xDD,0xCC,0xCC,0xBB,0xBB,0xCC,0xCC,0xCC,0xBB
	.byte 0xDE,0xCD,0xCC,0xCC,0xDD,0xCC,0xCC,0xCC,0xCD,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC
	.byte 0xCC,0xCC,0xCC,0xCC,0xCD,0xCC,0xCC,0xCC,0xDA,0xDD,0xCC,0xCC,0xAB,0xAA,0xDD,0xCC

	.byte 0xCC,0xCC,0xDC,0xCC,0xCC,0xCC,0xED,0xCD,0xCC,0xDC,0xEE,0xDE,0xDC,0xDC,0xEE,0xDE
	.byte 0xED,0xDD,0xEE,0xDE,0xED,0xDD,0xEE,0xDE,0xED,0xCD,0xED,0xCD,0xDC,0xCC,0xDC,0xCC
	.byte 0xDE,0xCC,0xCC,0xCC,0xCD,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC
	.byte 0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC
	.byte 0xCC,0xCC,0xCC,0xAB,0xCC,0xCC,0xCC,0xBC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC
	.byte 0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC
	.byte 0xBB,0xBB,0xBB,0xBB,0xBB,0xBB,0xBB,0xBB,0xBB,0xBB,0xBB,0xBB,0xBB,0xBB,0xBB,0xBB
	.byte 0xBB,0xBB,0xBB,0xBB,0xBB,0xBB,0xBB,0xBB,0xBC,0xBB,0xBB,0xBB,0xCC,0xBB,0xBB,0xBB

	.byte 0xCC,0xCC,0xCC,0xCC,0xDD,0xCC,0xCC,0xCC,0xAA,0xCD,0xCC,0xCC,0xBB,0xDA,0xCC,0xCC
	.byte 0xBB,0xAB,0xCD,0xCC,0xBB,0xBB,0xDA,0xCC,0xBB,0xBB,0xAB,0xCD,0xBB,0xBB,0xBB,0xDA
	.byte 0xBB,0xBB,0xBB,0xBB,0xBB,0xBB,0xBB,0xBB,0xBB,0xBB,0xBB,0xBB,0xBB,0xBB,0xBB,0xBB
	.byte 0xBB,0xBB,0xBB,0xBB,0xBB,0xBB,0xBB,0xBB,0xBB,0xBB,0xBB,0xBB,0xBB,0xBB,0xBB,0xBB
	.byte 0xBB,0xBB,0xBB,0xEB,0xBB,0xBB,0xBB,0xEE,0xBB,0xBB,0xEB,0xDE,0xBB,0xBB,0xEE,0xDD
	.byte 0xBB,0xEB,0xDE,0xCD,0xBB,0xEE,0xDD,0xCC,0xEB,0xDE,0xCD,0xCC,0xEE,0xDD,0xCC,0xCC
	.byte 0xCD,0xCC,0xCC,0xCC,0xDA,0xCC,0xCC,0xCC,0xAB,0xCD,0xCC,0xCC,0xBB,0xDA,0xCC,0xCC
	.byte 0xBB,0xAB,0xCD,0xCC,0xBB,0xBB,0xDA,0xCC,0xBB,0xBB,0xAB,0xCD,0xBB,0xBB,0xBB,0xDA

	.byte 0xBB,0xBB,0xBB,0xEB,0xBC,0xBB,0xBB,0xEE,0xBC,0xBB,0xEB,0xDE,0xCC,0xBB,0xEE,0xDD
	.byte 0xCC,0xEB,0xDE,0xCD,0xCC,0xEE,0xDD,0xCC,0xEC,0xDE,0xCD,0xCC,0xEE,0xDD,0xCC,0xCC
	.byte 0xCC,0xCC,0xCC,0xEC,0xCC,0xCC,0xCC,0xEE,0xCC,0xCC,0xEC,0xDE,0xCC,0xCC,0xEE,0xDD
	.byte 0xCC,0xEC,0xDE,0xCD,0xCC,0xEE,0xDD,0xCC,0xEC,0xDE,0xCD,0xCC,0xEE,0xDD,0xCC,0xCC
	.byte 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
	.byte 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
	.byte 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
	.byte 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00

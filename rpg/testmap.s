	.text
	.global testMap

testMap:
	.byte 20, 15 //size
	.long __testMap_Map
	.long officeTiles
	.long __testMap_Entities

__testMap_Map:
	.byte   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
	.byte   0,  0,  3,  3,  4,  5,  3,  3,  3,  4,  5,  3,  3,  3,  4,  5,  3,  3,  3,  0
	.byte   0,  0, 19, 19, 20, 21, 22, 32, 33, 20, 21, 22, 32, 33, 20, 21, 22, 19, 19,  0
	.byte   0,  0, 13, 35, 36, 37, 38, 35, 35, 36, 63, 38, 35,110, 36, 37, 38, 35, 14,  0
	.byte   0,  0, 23,  1,  1,  1,  1,  1,  1,  1, 27,  1,  1,114,  1,  1,  1,  1, 25,  0
	.byte   0,  0,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  0
	.byte   0,  0,  1,  1,  1,  1,  1,  1,  1,  1, 48, 49, 49, 49, 49, 49, 50,  1,  1,  0
	.byte   0,  0,  1,  1,  1,  1,  1,  1,  1,  1, 64, 65, 77, 78, 79,123, 66,  1,  1,  0
	.byte   0,  0,  1,  1, 52, 84,  1,  1,  9,  1, 64,121, 69, 70, 71,131, 66,  1,  1,  0
	.byte   0,  0,  0,  0,  0,  0,  0,  0, 25,  1, 64,129, 69, 70, 71, 65, 66,  1,  1,  0
	.byte   0,  0,  0,  0,  0,  0,  0,  0,  1,  1, 64, 75,137,139,140,123, 66,  1,  1,  0
	.byte   0,  0,  0,  0,  0,  0,  0,  0, 11,  1, 80, 81, 81, 81, 81,135, 82,  1, 96,  0
	.byte   0,  0,  0,  0,  0,  0,  0,  0, 27,  1,  1,  1,  1,  1,  1,  1,  1,  1,112,  0
	.byte   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
	.byte   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0

__testMap_Entities:
	.byte 2
//0
	.byte 1
	.byte 6, 6
	.byte 0
	.byte 255
	.byte 0
	.long 0
//1
	.byte 2
	.byte 8, 6 //13, 6
	.byte 0
	.byte 0
	.byte 0
	.long __testMap_Entity1_Script

__testMap_Entity1_Script:
	.byte 0x0D //push.b
	  .byte 0x04 //4
	.byte 0x11 //push1
	.byte 0x82 //face
	.byte 0x12 //push2
	.byte 0x11 //push1
	.byte 0x84 //portrait
	.byte 0x11 //push1
	.byte 0x0C //push.v
	  .byte 0x02 //2
	.byte 0x0F //push.s
	  .long __testMap_Entity1_8
	.byte 0x0D //push.b
	  .byte 0x03 //3
	.byte 0x81 //say
	.byte 0x0F //push.s
	  .long __testMap_Entity1_14
	.byte 0x0F //push.s
	  .long __testMap_Entity1_17
	.byte 0x12 //push2
	.byte 0x83 //ask
	.byte 0x11 //push1
	.byte 0x11 //push1
	.byte 0x84 //portrait
	.byte 0x0F //push.s
	  .long __testMap_Entity1_18
	.byte 0x11 //push1
	.byte 0x81 //say
	.byte 0x10 //push0
	.byte 0x11 //push1
	.byte 0x82 //face
	.byte 0xFF //end of script
__testMap_Entity1_8:
	.asciz "What a year, huh?"
__testMap_Entity1_14:
	.asciz "Ignore"
__testMap_Entity1_17:
	.asciz "Joke"
__testMap_Entity1_18:
	.asciz "It's only February."




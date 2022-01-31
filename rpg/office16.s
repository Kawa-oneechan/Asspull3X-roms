	.text

	.global officeTiles

officeTiles:
	.long tiles8
	.long palette
	.long tiles16

tiles8:
	.incbin "office8.img.bin"
palette:
	.incbin "office8.pal.bin"

#define SHALLOW	0x0010
#define DESK	0x0100
#define DANGER	0x1000
#define ONTOP	0x2000
#define SOLID	0x4000
#define WATER	0x8000

tiles16:
//0 void
	.short SOLID
	.short 0x003C,0x003C,0x003C,0x003C
	.short 0x0000,0x0000,0x0000,0x0000
//1 wood floor
	.short 0
	.short 0x1002,0x1003,0x1004,0x1005
	.short 0x0000,0x0000,0x0000,0x0000
//2 stone floor
	.short 0
	.short 0x3001,0x3001,0x3001,0x3001
	.short 0x0000,0x0000,0x0000,0x0000
//3 wall edge top
	.short SOLID
	.short 0x0000,0x0000,0x0006,0x0006
	.short 0x0000,0x0000,0x0000,0x0000
//4 wall edge top, column edge left
	.short SOLID
	.short 0x0000,0x0000,0x0006,0x0007
	.short 0x0000,0x0000,0x0000,0x0000
//5 wall edge top, column
	.short SOLID
	.short 0x0000,0x0000,0x0008,0x0009
	.short 0x0000,0x0000,0x0000,0x0000
//6 wall edge top on wood floor
	.short ONTOP
	.short 0x1002,0x1003,0x1004,0x1005
	.short 0x0000,0x0000,0x0006,0x0006
//7 top of plant 1 on wood floor
	.short ONTOP
	.short 0x1002,0x1003,0x1004,0x1005
	.short 0x0000,0x0000,0x204A,0x204B
//8 top of plant 1 on stone floor
	.short ONTOP
	.short 0x3001,0x3001,0x3001,0x3001
	.short 0x0000,0x0000,0x204A,0x204B
//9 top of plant 2 on wood floor
	.short ONTOP
	.short 0x1002,0x1003,0x1004,0x1005
	.short 0x0000,0x0000,0x204F,0x2050
//10 top of plant 2 on stone floor
	.short ONTOP
	.short 0x3001,0x3001,0x3001,0x3001
	.short 0x0000,0x0000,0x204F,0x2050
//11 top of plant 3 on wood floor
	.short ONTOP
	.short 0x1002,0x1003,0x1004,0x1005
	.short 0x2042,0x2043,0x2044,0x2045
//12 top of plant 3 on stone floor
	.short ONTOP
	.short 0x3001,0x3001,0x3001,0x3001
	.short 0x2042,0x2043,0x2044,0x2045
//13 top of plant 1 on wall
	.short SOLID
	.short 0x0012,0x0012,0x0014,0x0014
	.short 0x0000,0x0000,0x204A,0x204B
//14 top of plant 2 on wall
	.short SOLID
	.short 0x0012,0x0012,0x0014,0x0014
	.short 0x0000,0x0000,0x204F,0x2050
//15 top of plant 3 on wall
	.short SOLID
	.short 0x0012,0x0012,0x0014,0x0014
	.short 0x2042,0x2043,0x2044,0x2045
//16 wall edge top on wood floor
	.short ONTOP
	.short 0x3001,0x3001,0x3001,0x3001
	.short 0x0000,0x0000,0x0006,0x0006
//17 blank
	.short 0
	.short 0x0000,0x0000,0x0000,0x0000
	.short 0x0000,0x0000,0x0000,0x0000
//17 blank
	.short 0
	.short 0x0000,0x0000,0x0000,0x0000
	.short 0x0000,0x0000,0x0000,0x0000
//18 wall top half
	.short SOLID
	.short 0x000C,0x000C,0x0010,0x0010
	.short 0x0000,0x0000,0x0000,0x0000
//19 wall top half, column edge left
	.short SOLID
	.short 0x000C,0x000D,0x0010,0x0011
	.short 0x0000,0x0000,0x0000,0x0000
//20 column top half
	.short SOLID
	.short 0x000E,0x000F,0x0018,0x0019
	.short 0x0000,0x0000,0x0000,0x0000
//21 wall top half, column edge right
	.short SOLID
	.short 0x000A,0x000C,0x000B,0x0010
	.short 0x0000,0x0000,0x0000,0x0000
//22 bottom of plant 1 on wood floor
	.short SOLID
	.short 0x1002,0x1003,0x1004,0x1005
	.short 0x204C,0x204D,0x204E,0x2049
//23 bottom of plant 1 on stone floor
	.short SOLID
	.short 0x3001,0x3001,0x3001,0x3001
	.short 0x204C,0x204D,0x204E,0x2049
//24 bottom of plant 2 on wood floor
	.short SOLID
	.short 0x1002,0x1003,0x1004,0x1005
	.short 0x2051,0x2052,0x2053,0x2049
//25 bottom of plant 2 on stone floor
	.short SOLID
	.short 0x3001,0x3001,0x3001,0x3001
	.short 0x2051,0x2052,0x2053,0x2049
//26 bottom of plant 3 on wood floor
	.short SOLID
	.short 0x1002,0x1003,0x1004,0x1005
	.short 0x2046,0x2047,0x2048,0x2049
//27 bottom of plant 3 on stone floor
	.short SOLID
	.short 0x3001,0x3001,0x3001,0x3001
	.short 0x2046,0x2047,0x2048,0x2049
//28 top of plant 1 on wall, column edge left
	.short SOLID
	.short 0x0012,0x0013,0x0014,0x0015
	.short 0x0000,0x0000,0x204A,0x204B
//29 top of plant 2 on wall, column edge left
	.short SOLID
	.short 0x0012,0x0013,0x0014,0x0015
	.short 0x0000,0x0000,0x204F,0x2050
//30 top of plant 3 on wall, column edge left
	.short SOLID
	.short 0x0012,0x0013,0x0014,0x0015
	.short 0x2042,0x2043,0x2044,0x2045
//31 window left
	.short SOLID
	.short 0x101E,0x101F,0x1020,0x1021
	.short 0x0000,0x0000,0x0000,0x0000
//32 window right
	.short SOLID
	.short 0x141F,0x141E,0x1421,0x1420
	.short 0x0000,0x0000,0x0000,0x0000
//33 carpet 1x1
	.short 0
	.short 0x0033,0x0035,0x0039,0x003B
	.short 0x0000,0x0000,0x0000,0x0000
//34 wall bottom half
	.short SOLID
	.short 0x0012,0x0012,0x0014,0x0014
	.short 0x0000,0x0000,0x0000,0x0000
//35 wall bottom half, column edge left
	.short SOLID
	.short 0x0012,0x0013,0x0014,0x0015
	.short 0x0000,0x0000,0x0000,0x0000
//36 column bottom half
	.short SOLID
	.short 0x001A,0x001B,0x001C,0x001D
	.short 0x0000,0x0000,0x0000,0x0000
//37 wall bottom half, column edge right
	.short SOLID
	.short 0x0016,0x0012,0x0017,0x0014
	.short 0x0000,0x0000,0x0000,0x0000
//38 bottom of plant 1 on wood floor with bottom edge
	.short 0x6000
	.short 0x1002,0x1003,0x1004,0x1005
	.short 0x204C,0x204D,0x0006,0x0006
//39 bottom of plant 1 on stone floor with bottom edge
	.short 0x6000
	.short 0x3001,0x3001,0x3001,0x3001
	.short 0x204C,0x204D,0x0006,0x0006
//40 bottom of plant 2 on wood floor with bottom edge
	.short 0x6000
	.short 0x1002,0x1003,0x1004,0x1005
	.short 0x2051,0x2052,0x0006,0x0006
//41 bottom of plant 2 on stone floor with bottom edge
	.short 0x6000
	.short 0x3001,0x3001,0x3001,0x3001
	.short 0x2051,0x2052,0x0006,0x0006
//42 bottom of plant 3 on wood floor with bottom edge
	.short 0x6000
	.short 0x1002,0x1003,0x1004,0x1005
	.short 0x2046,0x2047,0x0006,0x0006
//43 bottom of plant 3 on stone floor with bottom edge
	.short 0x6000
	.short 0x3001,0x3001,0x3001,0x3001
	.short 0x2046,0x2047,0x0006,0x0006
//44 top of plant 1 on wall, column edge left
	.short SOLID
	.short 0x0016,0x0012,0x0017,0x0014
	.short 0x0000,0x0000,0x204A,0x204B
//45 top of plant 2 on wall, column edge left
	.short SOLID
	.short 0x0016,0x0012,0x0017,0x0014
	.short 0x0000,0x0000,0x204F,0x2050
//46 top of plant 3 on wall, column edge left
	.short SOLID
	.short 0x0016,0x0012,0x0017,0x0014
	.short 0x2042,0x2043,0x2044,0x2045
//47 carpet top left
	.short 0
	.short 0x0033,0x0034,0x0036,0x0037
	.short 0x0000,0x0000,0x0000,0x0000
//48 carpet top
	.short 0
	.short 0x0034,0x0034,0x0037,0x0037
	.short 0x0000,0x0000,0x0000,0x0000
//49 carpet top right
	.short 0
	.short 0x0034,0x0035,0x0037,0x0038
	.short 0x0000,0x0000,0x0000,0x0000
//50 carpet top left and right
	.short 0
	.short 0x0033,0x0035,0x0036,0x0038
	.short 0x0000,0x0000,0x0000,0x0000
//51 carpet left top and bottom
	.short 0
	.short 0x0033,0x0034,0x0039,0x003A
	.short 0x0000,0x0000,0x0000,0x0000
//52 table top left on wood
	.short ONTOP
	.short 0x1002,0x1003,0x1004,0x1005
	.short 0x0000,0x0000,0x1022,0x1023
//53 table top on wood
	.short ONTOP
	.short 0x1002,0x1003,0x1004,0x1005
	.short 0x0000,0x0000,0x1023,0x1023
//54 table top right on wood
	.short ONTOP
	.short 0x1002,0x1003,0x1004,0x1005
	.short 0x0000,0x0000,0x1023,0x1024
//55 table top left and right on wood
	.short ONTOP
	.short 0x1002,0x1003,0x1004,0x1005
	.short 0x0000,0x0000,0x1022,0x1024
//56 table top left on stone
	.short ONTOP
	.short 0x3001,0x3001,0x3001,0x3001
	.short 0x0000,0x0000,0x1022,0x1023
//57 table top on stone
	.short ONTOP
	.short 0x3001,0x3001,0x3001,0x3001
	.short 0x0000,0x0000,0x1023,0x1023
//58 table top right on stone
	.short ONTOP
	.short 0x3001,0x3001,0x3001,0x3001
	.short 0x0000,0x0000,0x1023,0x1024
//59 table top left and right on stone
	.short ONTOP
	.short 0x3001,0x3001,0x3001,0x3001
	.short 0x0000,0x0000,0x1022,0x1024
//60 top of plant 1 on wall, column edge left
	.short SOLID
	.short 0x001A,0x001B,0x001C,0x001D
	.short 0x0000,0x0000,0x204A,0x204B
//61 top of plant 2 on wall, column edge left
	.short SOLID
	.short 0x001A,0x001B,0x001C,0x001D
	.short 0x0000,0x0000,0x204F,0x2050
//62 top of plant 3 on wall, column edge left
	.short SOLID
	.short 0x001A,0x001B,0x001C,0x001D
	.short 0x2042,0x2043,0x2044,0x2045
//64 carpet left
	.short 0
	.short 0x0036,0x0037,0x0036,0x0037
	.short 0x0000,0x0000,0x0000,0x0000
//65 carpet middle
	.short 0
	.short 0x0037,0x0037,0x0037,0x0037
	.short 0x0000,0x0000,0x0000,0x0000
//66 carpet right
	.short 0
	.short 0x0037,0x0038,0x0037,0x0038
	.short 0x0000,0x0000,0x0000,0x0000
//67 carpet top middle and right
	.short 0
	.short 0x0036,0x0038,0x0036,0x0038
	.short 0x0000,0x0000,0x0000,0x0000
//68 carpet middle top and bottom
	.short 0
	.short 0x0034,0x0034,0x003A,0x003A
	.short 0x0000,0x0000,0x0000,0x0000
//69 table left
	.short SOLID
	.short 0x1025,0x1026,0x1025,0x1026
	.short 0x0000,0x0000,0x0000,0x0000
//70 table middle
	.short SOLID
	.short 0x1026,0x1026,0x1026,0x1026
	.short 0x0000,0x0000,0x0000,0x0000
//71 table right
	.short SOLID
	.short 0x1026,0x1027,0x1026,0x1027
	.short 0x0000,0x0000,0x0000,0x0000
//72 table middle left and right
	.short SOLID
	.short 0x1025,0x1027,0x1025,0x1027
	.short 0x0000,0x0000,0x0000,0x0000
//73 wastebasket on wood
	.short SOLID
	.short 0x1002,0x1003,0x1004,0x1005
	.short 0x3054,0x3055,0x3056,0x3057
//74 wastebasket on stone
	.short SOLID
	.short 0x3001,0x3001,0x3001,0x3001
	.short 0x3054,0x3055,0x3056,0x3057
//75 wastebasket on carpet
	.short SOLID
	.short 0x0037,0x0037,0x0037,0x0037
	.short 0x3054,0x3055,0x3056,0x3057
//76 table top left and right on carpet
	.short ONTOP
	.short 0x0037,0x0037,0x0037,0x0037
	.short 0x0000,0x0000,0x1022,0x1024
//77 table top left on carpet
	.short ONTOP
	.short 0x0037,0x0037,0x0037,0x0037
	.short 0x0000,0x0000,0x1022,0x1023
//78 table top on carpet
	.short ONTOP
	.short 0x0037,0x0037,0x0037,0x0037
	.short 0x0000,0x0000,0x1023,0x1023
//79 table top right on carpet
	.short ONTOP
	.short 0x0037,0x0037,0x0037,0x0037
	.short 0x0000,0x0000,0x1023,0x1024
//80 carpet bottom left
	.short 0
	.short 0x0036,0x0037,0x0039,0x003A
	.short 0x0000,0x0000,0x0000,0x0000
//81 carpet bottom
	.short 0
	.short 0x0037,0x0037,0x003A,0x003A
	.short 0x0000,0x0000,0x0000,0x0000
//82 carpet bottom right
	.short 0
	.short 0x0037,0x0038,0x003A,0x003B
	.short 0x0000,0x0000,0x0000,0x0000
//83 carpet bottom middle and right
	.short 0
	.short 0x0036,0x0038,0x0039,0x003B
	.short 0x0000,0x0000,0x0000,0x0000
//84 carpet right top and bottom
	.short 0
	.short 0x0034,0x0035,0x003A,0x003B
	.short 0x0000,0x0000,0x0000,0x0000
//85 table bottom left and right on wood
	.short SOLID
	.short 0x1002,0x1003,0x1004,0x1005
	.short 0x1028,0x1428,0x102E,0x142E
//86 table bottom left on wood
	.short SOLID
	.short 0x1002,0x1003,0x1004,0x1005
	.short 0x1029,0x102D,0x102F,0x0000
//87 table bottom on wood
	.short SOLID
	.short 0x1002,0x1003,0x1004,0x1005
	.short 0x102D,0x102D,0x0000,0x0000
//88 table bottom right left on wood
	.short SOLID
	.short 0x1002,0x1003,0x1004,0x1005
	.short 0x102D,0x102A,0x0000,0x1030
//89 table bottom right right on wood
	.short SOLID
	.short 0x1002,0x1003,0x1004,0x1005
	.short 0x102B,0x102C,0x1031,0x1032
//90 table bottom left and right on stone
	.short SOLID
	.short 0x3001,0x3001,0x3001,0x3001
	.short 0x1028,0x1428,0x102E,0x142E
//91 table bottom left on stone
	.short SOLID
	.short 0x3001,0x3001,0x3001,0x3001
	.short 0x1029,0x102D,0x102F,0x0000
//92 table bottom on stone
	.short SOLID
	.short 0x3001,0x3001,0x3001,0x3001
	.short 0x102D,0x102D,0x0000,0x0000
//93 table bottom right left on stone
	.short SOLID
	.short 0x3001,0x3001,0x3001,0x3001
	.short 0x102D,0x102A,0x0000,0x1030
//94 table bottom right right on stone
	.short SOLID
	.short 0x3001,0x3001,0x3001,0x3001
	.short 0x102B,0x102C,0x1031,0x1032
//95 blank
	.short 0
	.short 0x0000,0x0000,0x0000,0x0000
	.short 0x0000,0x0000,0x0000,0x0000
//96 chair up on wood
	.short ONTOP
	.short 0x1002,0x1003,0x1004,0x1005
	.short 0x0000,0x0000,0x3058,0x3458
//97 chair right on wood
	.short ONTOP
	.short 0x1002,0x1003,0x1004,0x1005
	.short 0x0000,0x0000,0x3059,0x0000
//98 chair down on wood
	.short ONTOP
	.short 0x1002,0x1003,0x1004,0x1005
	.short 0x0000,0x0000,0x305A,0x345A
//99 chair left on wood
	.short ONTOP
	.short 0x1002,0x1003,0x1004,0x1005
	.short 0x0000,0x0000,0x0000,0x3459
//96 chair up on stone
	.short ONTOP
	.short 0x3001,0x3001,0x3001,0x3001
	.short 0x0000,0x0000,0x3058,0x3458
//97 chair right on stone
	.short ONTOP
	.short 0x3001,0x3001,0x3001,0x3001
	.short 0x0000,0x0000,0x3059,0x0000
//98 chair down on stone
	.short ONTOP
	.short 0x3001,0x3001,0x3001,0x3001
	.short 0x0000,0x0000,0x305A,0x345A
//99 chair left on stone
	.short ONTOP
	.short 0x3001,0x3001,0x3001,0x3001
	.short 0x0000,0x0000,0x0000,0x3459
//100 chair up on table
	.short SOLID
	.short 0x1026,0x1026,0x1026,0x1026
	.short 0x0000,0x0000,0x3058,0x3458
//101 chair right on table
	.short SOLID
	.short 0x1026,0x1026,0x1026,0x1026
	.short 0x0000,0x0000,0x3059,0x0000
//102 chair down on table
	.short SOLID
	.short 0x1026,0x1026,0x1026,0x1026
	.short 0x0000,0x0000,0x305A,0x345A
//103 chair left on table
	.short SOLID
	.short 0x1026,0x1026,0x1026,0x1026
	.short 0x0000,0x0000,0x0000,0x3459
//104 chair up on wall
	.short SOLID
	.short 0x0012,0x0012,0x0014,0x0014
	.short 0x0000,0x0000,0x3058,0x3458
//105 chair right on wall
	.short SOLID
	.short 0x0012,0x0012,0x0014,0x0014
	.short 0x0000,0x0000,0x3059,0x0000
//106 chair down on wall
	.short SOLID
	.short 0x0012,0x0012,0x0014,0x0014
	.short 0x0000,0x0000,0x305A,0x345A
//107 chair left on wall
	.short SOLID
	.short 0x0012,0x0012,0x0014,0x0014
	.short 0x0000,0x0000,0x0000,0x3459
//108 chair bottom up on wood
	.short SOLID
	.short 0x1002,0x1003,0x1004,0x1005
	.short 0x305B,0x345B,0x305F,0x345F
//109 chair bottom right on wood
	.short SOLID
	.short 0x1002,0x1003,0x1004,0x1005
	.short 0x305C,0x305D,0x305F,0x345F
//110 chair bottom down on wood
	.short SOLID
	.short 0x1002,0x1003,0x1004,0x1005
	.short 0x305E,0x345E,0x305F,0x345F
//111 chair bottom left on wood
	.short SOLID
	.short 0x1002,0x1003,0x1004,0x1005
	.short 0x345D,0x345C,0x305F,0x345F
//112 chair bottom up on stone
	.short SOLID
	.short 0x3001,0x3001,0x3001,0x3001
	.short 0x305B,0x345B,0x305F,0x345F
//113 chair bottom right on stone
	.short SOLID
	.short 0x3001,0x3001,0x3001,0x3001
	.short 0x305C,0x305D,0x305F,0x345F
//114 chair bottom down on stone
	.short SOLID
	.short 0x3001,0x3001,0x3001,0x3001
	.short 0x305E,0x345E,0x305F,0x345F
//115 chair bottom left on stone
	.short SOLID
	.short 0x3001,0x3001,0x3001,0x3001
	.short 0x345D,0x345C,0x305F,0x345F
//116 chair up on carpet
	.short ONTOP
	.short 0x0037,0x0037,0x0037,0x0037
	.short 0x0000,0x0000,0x3058,0x3458
//117 chair right on carpet
	.short ONTOP
	.short 0x0037,0x0037,0x0037,0x0037
	.short 0x0000,0x0000,0x3059,0x0000
//118 chair down on carpet
	.short ONTOP
	.short 0x0037,0x0037,0x0037,0x0037
	.short 0x0000,0x0000,0x305A,0x345A
//119 chair left on carpet
	.short ONTOP
	.short 0x0037,0x0037,0x0037,0x0037
	.short 0x0000,0x0000,0x0000,0x3459
//120 chair up on carpet top
	.short ONTOP
	.short 0x0034,0x0034,0x0037,0x0037
	.short 0x0000,0x0000,0x3058,0x3458
//121 chair right on carpet top
	.short ONTOP
	.short 0x0034,0x0034,0x0037,0x0037
	.short 0x0000,0x0000,0x3059,0x0000
//122 chair down on carpet top
	.short ONTOP
	.short 0x0034,0x0034,0x0037,0x0037
	.short 0x0000,0x0000,0x305A,0x345A
//123 chair left on carpet top
	.short ONTOP
	.short 0x0034,0x0034,0x0037,0x0037
	.short 0x0000,0x0000,0x0000,0x3459
//124 chair bottom up on carpet
	.short SOLID
	.short 0x0037,0x0037,0x0037,0x0037
	.short 0x305B,0x345B,0x305F,0x345F
//125 chair bottom right on carpet
	.short SOLID
	.short 0x0037,0x0037,0x0037,0x0037
	.short 0x305C,0x305D,0x305F,0x345F
//126 chair bottom down on carpet
	.short SOLID
	.short 0x0037,0x0037,0x0037,0x0037
	.short 0x305E,0x345E,0x305F,0x345F
//127 chair bottom left on carpet
	.short SOLID
	.short 0x0037,0x0037,0x0037,0x0037
	.short 0x345D,0x345C,0x305F,0x345F
//128 chair bottom up on carpet bottom
	.short SOLID
	.short 0x0037,0x0037,0x003A,0x003A
	.short 0x305B,0x345B,0x305F,0x345F
//129 chair bottom right on carpet bottom
	.short SOLID
	.short 0x0037,0x0037,0x003A,0x003A
	.short 0x305C,0x305D,0x305F,0x345F
//130 chair bottom down on carpet bottom
	.short SOLID
	.short 0x0037,0x0037,0x003A,0x003A
	.short 0x305E,0x345E,0x305F,0x345F
//131 chair bottom left on carpet bottom
	.short SOLID
	.short 0x0037,0x0037,0x003A,0x003A
	.short 0x345D,0x345C,0x305F,0x345F
//132 table bottom left and right on carpet
	.short SOLID
	.short 0x0037,0x0037,0x0037,0x0037
	.short 0x1028,0x1428,0x102E,0x142E
//133 table bottom left on carpet
	.short SOLID
	.short 0x0037,0x0037,0x0037,0x0037
	.short 0x1029,0x102D,0x102F,0x0000
//134 table bottom on carpet
	.short SOLID
	.short 0x0037,0x0037,0x0037,0x0037
	.short 0x102D,0x102D,0x0000,0x0000
//135 table bottom right left on carpet
	.short SOLID
	.short 0x0037,0x0037,0x0037,0x0037
	.short 0x102D,0x102A,0x0000,0x1030
//136 table bottom right right on carpet
	.short SOLID
	.short 0x0037,0x0037,0x0037,0x0037
	.short 0x102B,0x102C,0x1031,0x1032
//137 blank
	.short 0
	.short 0x0000,0x0000,0x0000,0x0000
	.short 0x0000,0x0000,0x0000,0x0000
//138 blank
	.short 0
	.short 0x0000,0x0000,0x0000,0x0000
	.short 0x0000,0x0000,0x0000,0x0000
//139 blank
	.short 0
	.short 0x0000,0x0000,0x0000,0x0000
	.short 0x0000,0x0000,0x0000,0x0000


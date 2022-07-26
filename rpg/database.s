	.global party, opponentDB, formationsDB
	.text

party:
//Farah
	.long 0x00000000
	.short 50, 50
	.short 20, 20
	.asciz "Farah"; .skip 7
	.byte 0
//Sara
	.long 0x00000000
	.short 30, 30
	.short 20, 20
	.asciz "Sara"; .skip 8
	.byte 0
//Laura
	.long 0x00000000
	.short 40, 40
	.short 20, 20
	.asciz "Laura"; .skip 7
	.byte 0
//filler
	.skip 130

opponentDB:
//Ratte
	.long 0x00000000
	.short 10, 10
	.asciz "Ratte"; .skip 10
	.long 0
//Invader
	.long 0x00000000
	.short 20, 10
	.asciz "Invader"; .skip 8
	.long 0

#define mRATTE 1
#define mINVADER 2

formationsDB:
	.byte mRATTE; .skip 5
	.byte mRATTE, mRATTE; .skip 4
	.byte mINVADER; .skip 5
	.byte mINVADER, mINVADER; .skip 4
	.byte mINVADER, mRATTE; .skip 4


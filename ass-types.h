#if !defined ASSTYPES
#define ASSTYPES
typedef signed char int8_t;
typedef short int16_t;
typedef int int32_t;

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;

typedef signed long intptr_t;
typedef unsigned long uintptr_t;

typedef unsigned long size_t;
typedef signed long ptrdiff_t;

#ifndef __bool_true_false_are_defined
	#define bool _Bool
	#define true ((_Bool)+1u)
	#define false ((_Bool)+0u)
	#define __bool_true_false_are_defined
#endif

/*
//typedef int int;
typedef long int32;
typedef short int16;
typedef char sbyte;
typedef unsigned int uint;
typedef unsigned long uint32;
typedef unsigned short uint16;
typedef unsigned char byte;
typedef unsigned long uint32;
typedef volatile int vint;
typedef volatile long vint32;
typedef volatile short vint16;
typedef volatile char vsbyte;
typedef volatile unsigned int vuint;
typedef volatile unsigned long vuint32;
typedef volatile unsigned short vuint16;
typedef volatile unsigned char vbyte;
*/
#endif

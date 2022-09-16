#include "../ass.h"
IBios* interface;

/*
#include <limits.h>

#define FRACBITS 16
#define FRACUNIT (1 << FRACBITS)
typedef int fixed_t;

int abs(int a)
{
	return a < 0 ? -a : a;
}

fixed_t FixedMul(fixed_t a, fixed_t b)
{
	return ((long long) a * (long long) b) >> FRACBITS;
}

fixed_t FixedDiv(fixed_t a, fixed_t b)
{
	if ((abs(a) >> 14) >= abs(b))
	return (a ^ b) < 0 ? INT_MIN : INT_MAX;

	long long c;
	c = ((long long)a <<16 ) / ((long long)b);
	return (fixed_t)c;
}

inline fixed_t Fixed(int a, int b)
{
	fixed_t c = (a << FRACBITS) + ((b * FRACUNIT) / 100);
	return c;
}

void printfixnum(fixed_t a)
{
	printf("%d.%d", a >> FRACBITS, ((a % FRACUNIT) * 100) / FRACUNIT);
}
*/

int main(void)
{
	MISC->SetTextMode(SMODE_240 | SMODE_BOLD);
	TEXT->SetTextColor(1, 7);
	TEXT->ClearScreen();
	printf("Hello, world!\n");

/*
	TEXT->SetTextColor(1, 7);
	printf("\nFRACUNIT is %d\n", FRACUNIT);
	fixed_t f1 = Fixed(12, 50); //(12 * FRACUNIT) + (FRACUNIT >> 1);
	fixed_t f2 = Fixed(2, 0); //2 * FRACUNIT;
	printf("%d -> ", f1);
	printfixnum(f1);
	printf("\n");
	printf("%d -> ", f2);
	printfixnum(f2);
	printf("\n");
	fixed_t f3 = FixedDiv(f1, f2);
	printf("Divide and... %d -> ", f3);
	printfixnum(f3);
	printf("\n");
	f3 = FixedMul(f1, f2);
	printf("Multiply and... %d -> ", f3);
	printfixnum(f3);
	printf("\n");
*/

	TEXT->SetTextColor(1, 15);
	printf("\nA\x11\xD7 BIOS/Interface\n");
	TEXT->SetTextColor(1, 7);
	printf("Version: %d.%d\t\t", (interface->biosVersion >> 8) & 0xFF, (interface->biosVersion >> 0) & 0xFF);
	printf("Extensions: %d\n", interface->extensions);
	printf("Printer: %#08X\t", interface->LinePrinter);
	printf("Disk drives: %d\n", interface->io.numDrives);

	TEXT->SetTextColor(1, 15);
	printf("\nSizes:\n");
	TEXT->SetTextColor(1, 7);
	printf("char        %d\t", sizeof(char));
	printf("short       %d\n", sizeof(short));
	printf("int         %d\t", sizeof(int));
	printf("long        %d\n", sizeof(long));
	printf("void *      %d\t", sizeof(void*));
	printf("long long   %d\n", sizeof(long long));
	printf("time_t      %d\n", sizeof(time_t));

	TEXT->SetTextColor(1, 15);
	printf("\nDefines:\n");
	TEXT->SetTextColor(1, 7);
#ifdef __STDC__
	printf("__STDC__\n");
	#ifdef __STDC_VERSION__
		printf("__STDC_VERSION__: %d ("
		#if __STDC_VERSION__ >= 201710L
			"C18"
		#elif __STDC_VERSION__ >= 201112L
			"C11"
		#elif __STDC_VERSION__ >= 199901L
			"C99"
		#elif __STDC_VERSION__ >= 199409L
			"C94"
		#else
			"???"
		#endif
		")\n", __STDC_VERSION__);
	#endif
#endif
#ifdef __cplusplus
	printf("__cplusplus: %d ("
	#if __cplusplus  >= 201703L
		"C++17"
	#elif __cplusplus  >= 201402L
		"C++14"
	#elif __cplusplus  >= 201103L
		"C++11"
	#elif __cplusplus  >= 199711L
		"C++98"
	#else
		"???"
	#endif
	")\n", __cplusplus);
#else
	printf("No __cplusplus\n");
#endif
#ifdef _POSIX_VERSION
	printf("_POSIX_VERSION: %d ("
	#if _POSIX_VERSION  >= 200809L
		"POSIX.1-2008"
	#elif _POSIX_VERSION  >= 200112L
		"POSIX.1-2001"
	#elif _POSIX_VERSION  >= 199506L
		"POSIX.1-1996"
	#elif _POSIX_VERSION  >= 199309L
		"POSIX.1-1993"
	#elif _POSIX_VERSION  >= 199009L
		"POSIX.1-1990"
	#elif _POSIX_VERSION  >= 198808L
		"POSIX.1-1988"
	#else
		"???"
	#endif
	")\n", _POSIX_VERSION);
#else
	printf("No _POSIX_VERSION\n");
#endif
	printf("Compiler: "
#ifdef __clang__
	"clang\n"
#elif defined(__GNUC__)
	"gcc"
	#ifdef __GNUC_MINOR__
		" %d.%d.%d\n", __GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__
	#endif
#else
	"unknown\n"
#endif
	);
	printf("Architecture: "
#ifdef __m68k__
	"Motorola "
	#ifdef __mc68040__
		"68040"
	#elif defined __mc68030__
		"68030"
	#elif defined __mc68020__
		"68020"
	#elif defined __mc68010__
		"68010"
	#elif defined __mc68000__
		"68000"
	#endif
#else
	"unknown"
#endif
	"\n"
	);

	TEXT->SetTextColor(1, 15);
	printf("\nLocale\n");
	TEXT->SetTextColor(1, 7);
	printf("ID code: \"%s\". ", MISC->GetLocaleStr(LC_CODE, 0));
	printf("Monday: \"%s\"\n", MISC->GetLocaleStr(LC_DAYF, 1));
	printf("January: \"%s\". ", MISC->GetLocaleStr(LC_MONF, 0));
	printf("June: \"%s\"\n", MISC->GetLocaleStr(LC_MONF, 5));
	printf("Short date: \"%s\". ", MISC->GetLocaleStr(LC_DATES, 0));
	printf("Long date: \"%s\"\n", MISC->GetLocaleStr(LC_DATEL, 0));
	printf("Short time: \"%s\"\n", MISC->GetLocaleStr(LC_TIMES, 0));
	printf("Long time: \"%s\"\n", MISC->GetLocaleStr(LC_TIMEL, 0));
	printf("Separators: '%c' and '%c'. ", interface->locale.thousands, interface->locale.decimals);
	printf("Currency: \"%s\"\n", MISC->GetLocaleStr(LC_CURR, 0));
}

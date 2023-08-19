#include "../ass.h"

extern void WaitForKey();

void MiscInfo()
{
	TEXT->ClearScreen();
	TEXT->Write("System Information\n\n");

	TEXT->Write("A\x11\xD7 BIOS Interface\n\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\n");
	TEXT->Write("Version: %d.%d\t\t", (interface->biosVersion >> 8) & 0xFF, (interface->biosVersion >> 0) & 0xFF);
	TEXT->Write("Extensions: %d\n", interface->extensions);
	TEXT->Write("Printer: %#08X\t", interface->linePrinter);
	TEXT->Write("Disk drives: %d (see System Info)\n", interface->io.numDrives);

	if (interface->linePrinter)
		TEXT->Write("The printer should be listed as #%d in System Information.\n", ((int)interface->linePrinter - MEM_DEVS) / 0x8000);
	if (interface->extensions)
		TEXT->Write("I'm not sure how you can have a non-zero Extensions field -- Kawa\n");
	else
		TEXT->Write("An Extensions field of zero would indicate there are BIOS extensions.\n");

	TEXT->Write("\nLocale\n\x90\x90\x90\x90\x90\x90\n");
	TEXT->Write("ID code: \"%s\"\n", MISC->GetLocaleStr(LC_CODE, 0));
	TEXT->Write("Days: \"%s\" or \"%s\"\n", MISC->GetLocaleStr(LC_DAYS, 1), MISC->GetLocaleStr(LC_DAYF, 1));
	TEXT->Write("Months: \"%s\" or \"%s\"\n", MISC->GetLocaleStr(LC_MONS, 0), MISC->GetLocaleStr(LC_MONF, 0));
	TEXT->Write("Dates: \"%s\" or \"%s\"\n", MISC->GetLocaleStr(LC_DATES, 0), MISC->GetLocaleStr(LC_DATEL, 0));
	TEXT->Write("Times: \"%s\" or \"%s\"\n", MISC->GetLocaleStr(LC_TIMES, 0), MISC->GetLocaleStr(LC_TIMEL, 0));
	TEXT->Write("Separators: '%c' and '%c'\n", interface->locale.thousands, interface->locale.decimals);
	TEXT->Write("Currency: \"%s\"\n", MISC->GetLocaleStr(LC_CURR, 0));

/*
	//Not really something in the system's control.
	//For reference:
	// bool           1    8
	// char           1    8
	// short          2   16
	// float          4   32
	// long           4   32
	// int            4   32
	// void*          4   32
	// double         8   64
	// long long      8   64
	// time_t         8   64
	// long double   12   96

	TEXT->Write("\nBasic Sizes (bytes, bits)\n\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\n");
	TEXT->Write("char        %d, %d\t", sizeof(char), sizeof(char) * 8);
	TEXT->Write("short       %d, %d\t", sizeof(short), sizeof(short) * 8);
	TEXT->Write("int         %d, %d\n", sizeof(int), sizeof(int) * 8);
	TEXT->Write("long        %d, %d\t", sizeof(long), sizeof(long) * 8);
	TEXT->Write("void*       %d, %d\t", sizeof(void*), sizeof(void*) * 8);
	TEXT->Write("long long   %d, %d\n", sizeof(long long), sizeof(long long) * 8);
	TEXT->Write("time_t      %d, %d\t", sizeof(time_t), sizeof(time_t) * 8);
	TEXT->Write("float       %d, %d\t", sizeof(float), sizeof(float) * 8);
	TEXT->Write("double      %d, %d\n", sizeof(double), sizeof(double) * 8);
	TEXT->Write("long double %d, %d\t", sizeof(long double), sizeof(long double) * 8);
	TEXT->Write("bool        %d, %d\t", sizeof(bool), sizeof(bool) * 8);
*/

	WaitForKey();
}

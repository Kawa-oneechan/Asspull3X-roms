#include "../ass.h"
#include "helpers.h"
extern void srand(unsigned int seed);
extern unsigned int rand();

void shuffle_array(char *array, unsigned int nelements)
{
	unsigned int i, j, tmp;

	for (i = 0; i < nelements; i++)
	{
		j = rand() % nelements;
		tmp = array[j];
		array[j] = array[i];
		array[i] = tmp;
	}
}

int is_in_array(int target, const int *array, unsigned int nelements)
{
	for (unsigned i = 0; i < nelements; i++)
		if (array[i] == target)
			return 1;

	return 0;
}

void set_array(int value, int *array, unsigned int nelements)
{
	for (unsigned i = 0; i < nelements; i++)
		array[i] = value;
}

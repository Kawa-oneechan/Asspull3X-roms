#include "../ass.h"
#include "helpers.h"
extern void srand(uint32_t seed);
extern uint32_t rand();

void shuffle_array(char *array, size_t nelements)
{
	uint32_t i, j, tmp;

	for (i = 0; i < nelements; i++)
	{
		j = rand() % nelements;
		tmp = array[j];
		array[j] = array[i];
		array[i] = tmp;
	}
}

int is_in_array(int target, const int *array, size_t nelements)
{
	for (uint32_t i = 0; i < nelements; i++)
		if (array[i] == target)
			return 1;

	return 0;
}

void set_array(int value, int *array, size_t nelements)
{
	for (uint32_t i = 0; i < nelements; i++)
		array[i] = value;
}

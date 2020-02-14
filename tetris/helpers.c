#include "../ass.h"
#include "helpers.h"
extern void srand(unsigned int seed);
extern unsigned int rand();


void *safe_malloc(size_t size)
{
        void *ptr = malloc(size);
/*        if (ptr == NULL) {
                fprintf(stderr, "error: cannot allocate %zu bytes of memory\n",
                        size);
                exit(EXIT_FAILURE);
        }
*/        return ptr;
}

void shuffle_array(char *array, unsigned int nelements)
{
        int i, j, tmp;

        for (i = 0; i < nelements; i++) {

                j = rand() % nelements;
                tmp = array[j];
                array[j] = array[i];
                array[i] = tmp;
        }
}

int is_in_array(int target, const int *array, unsigned int nelements)
{
        for (int i = 0; i < nelements; i++) {
                if (array[i] == target) {
                        return 1;
                }
        }
        return 0;
}

void set_array(int value, int *array, unsigned int nelements)
{
        for (int i = 0; i < nelements; i++) {
                array[i] = value;
        }
}

#ifndef HELPERS_HEADER
#define HELPERS_HEADER

#include <stddef.h>
#include <stdint.h>

/*
 * safe_malloc:  allocates size bytes and returns a void pointer to the
 *               allocated memory. On failure, prints an error end calls
 *               exit(EXIT_FAILURE).
 */
void *safe_malloc(size_t size);

/*
 * shuffle_array:  changes position of elements in array randomly.
 *                 Uses rand(), so srand() should be called before.
 */
void shuffle_array(char *array, size_t nelements);

/*
 * is_in_array:  returns true if target is contained (at least once) in array,
 *               false otherwise.
 */
int is_in_array(int target, const int *array, size_t nelements);

/*
 * set_array:  overwrite each element of array with value.
 */
void set_array(int value, int *array, size_t nelements);

#endif

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "trie.h"

int compare_int(void *i1, void *i2) {
	return *(*(int **) i1) == *(*(int **) i2);
}

// using something like an array necisitates having some sort of null terminator
// This version uses a 0:
void *next_int(void *i) {
	if (*(*(int **) i) == 0)
		return NULL;

	return (int **) i + sizeof(int *);
}

int delete_int(void *i) {
	free(*(int **) i);

	return 0;
}

int test() {
	/* UNSTRUCTURED INPUT */
	trie_t *trie_first = trie_create("-pc");

	trie_insert(trie_first, "cat");
	trie_insert(trie_first, "bab");
	trie_insert(trie_first, "ace");
	trie_insert(trie_first, "ace");

	assert(trie_search(trie_first, "cow") == 0);
	assert(trie_search(trie_first, "bab") == 1);
	assert(trie_search(trie_first, "ace") == 2);

	trie_destroy(trie_first);

	/* UNSTRUCTURED INPUT -- using int ** */
	trie_t *trie_second = trie_create("-pv -c -n -d", compare_int, next_int, delete_int);

	int **test1 = malloc(sizeof(int *) * 4);
	for (int add_to = 3; add_to >= 0; add_to--) {
		test1[add_to] = malloc(sizeof(int));
		*test1[add_to] = add_to;
	}
	trie_insert(trie_second, test1);

	return 0;
}

int main() {
	test();

	printf("\nALL TESTS PASSED\n");

	return 0;
}
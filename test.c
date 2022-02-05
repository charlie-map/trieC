#include <stdio.h>
#include <stdlib.h>

#include "trie.h"

int main() {
	trie_t *test_trie = trie_create("");

	trie_insert(test_trie, "cat");
	int test = trie_search(test_trie, "cat");

	printf("%d\n", test);
	trie_insert(test_trie, "bab");
	int test2 = trie_search(test_trie, "bab");
	int test3 = trie_search(test_trie, "ace");

	printf("test 2: %d\n", test2);
	printf("test 3: %d\n", test3);

	trie_destroy(test_trie);

	return 0;
}
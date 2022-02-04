#include <stdio.h>
#include <stdlib.h>

#include "trie.h"

int main() {
	trie_t *test_trie = trie_create("");

	trie_insert(test_trie, "cat");

	return 0;
}
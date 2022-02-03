#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "trie.h"

// defualt weight function
int default_weight(char payload1, char payload2) {
	return (int) payload1 < (int) payload2;
}

typedef struct TrieNode {
	char simple_payload;
	void *payload;

	int thru_weight;
	int end_weight;

	// circularly linked list
	struct TrieNode *next, *prev;
	struct TrieNode *chilren; // pointer to lower circularly linked list
} node_t;

struct Trie {
	int (*simple_weight)(char, char); // choose which child to explore
	int (*heavy_weight)(void *, void *);

	int weight_option; // chooses payload type (char or void *),
					   // 1 for simple_weight (char), 0 for void *
	int children_option; // number of children per level

	node_t *root_node;
};

/*
	trie_create develops a new header for a trie. The header connects into
	a root node that contains the actual root node of the trie

	trie_create takes in the parameter: param
	- param will include the structure of any included values such as (per 1.0):
		'w': weighting function, should take in two void * (or char) values and return
		either 1 for the first value is greater, negative one if less, and 0
		for if the values are equal. If not given, the assumed weight default
		function will use char *'s as per default trie behavior
		'p': payload type, defines what kind of data the trie holds: either
		char (if given 'c') or void * (if given 'v')

		param will look something like:
			"-w -pc", int (*weight)(char, char)
		or:
			"-w -pv", int (*weight)(void *, void *)
*/
trie_t *trie_create(char *param, ...) {
	trie_t *new_trie = malloc(sizeof(trie_t));

	new_trie->heavy_weight = NULL;
	new_trie->simple_weight = NULL;
	new_trie->children_option = 0;
	new_trie->root_node = NULL;

	va_list param_detail;
	va_start(param_detail, param);

	// look at p_tag first
	int find_p = 0;
	while (param[find_p] && param[find_p + 1] && (param[find_p] != '-' || param[find_p + 1] != 'p')) {
		find_p++;
	}

	if (param[find_p] && param[find_p + 1] && param[find_p] == '-' && param[find_p + 1]) {
		if (!param[find_p + 2]) // look for value
			return NULL; // ERROR

		new_trie->weight_option = param[find_p + 2] == 'c';
	} else
		new_trie->weight_option = 1;

	// check for other parameters
	for (find_p = 0; param[find_p + 1]; find_p++) {
		if (param[find_p] != '-' || param[find_p] != ' ')
			continue;

		if (param[find_p + 1] == 'w') {
			if (new_trie->weight_option)
				new_trie->simple_weight = va_arg(param_detail, int (*)(char, char));
			else
				new_trie->heavy_weight = va_arg(param_detail, int (*)(void *, void *));
		}
	}

	if (new_trie->weight_option && !new_trie->simple_weight) // default action
		new_trie->simple_weight = default_weight;

	// return updated new_trie
	return new_trie;
}

int trie_insert(trie_t *trie, ...) {
	
}

/*
	Destruct takes in the Trie *trie HEAD and DFS through to
	free all of the allocated heap memory

	After this function the trie will be fully empty (including the head)
*/
int destruct(Trie *trie) {

	if (!trie->childCount) {
		free(trie);
		return 0;
	}

	for (int i = 0; i < 26; i++) {
		if (trie->children[i])
			destruct(trie->children[i]);
	}

	free(trie);
	return 0;
}
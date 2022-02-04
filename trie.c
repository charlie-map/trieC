#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "trie.h"

char simple_convert(void *payload) {
	return ((char *) payload)[0];
}

// defualt weight function
int default_weight(char payload1, char payload2) {
	return (int) payload1 < (int) payload2;
}

typedef struct TrieNode {
	void *payload;

	int thru_weight;
	int end_weight;

	// circularly linked list
	struct TrieNode *next, *prev;
	struct TrieNode *chilren; // pointer to lower circularly linked list
} node_t;

node_t *node_construct(void *payload) {
	node_t *new_node = malloc(sizeof(node_t));

	new_node->payload = payload;

	new_node->thru_weight = 0;
	new_node->end_weight = 0;

	// circularly link
	new_node->next = new_node;
	new_node->prev = new_node
	new_node->children = NULL;

	return new_node;
}

typedef struct Weight {
	int weight_option; // chooses payload type (char or void *),
				       // 1 for simple_weight (char), 0 for void *

	char (*simple_convert)(void *);
	int (*simple_weight)(char, char); // choose which child to explore
	int (*heavy_weight)(void *, void *);
} weight_param;

struct Trie {
	weight_param weight_obj;

	int (*weight)(struct Weight, void *, void *);

	int children_option; // number of children per level

	node_t *root_node;
};

int major_weight(weight_param weight_obj, void *p1, void *p2) {
	// pick based on weight_obj.weight_option

	char c_p1, c_p2;

	if (weight_obj.weight_option) {
		c_p1 = weight_obj.simple_convert(p1);
		c_p2 = weight_obj.simple_convert(p2);

		return weight_obj.simple_weight(c_p1, c_p2);
	} else
		return weight_obj.heavy_weight(p1, p2);
}

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

	// setup weight
	weight_param weight_scheme;
	weight_scheme.simple_convert = simple_convert;
	weight_scheme.simple_weight = NULL;
	weight_scheme.heavy_weight = NULL;

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

		weight_param.weight_option = param[find_p + 2] == 'c';
	} else
		weight_param.weight_option = 1;

	// check for other parameters
	for (find_p = 0; param[find_p + 1]; find_p++) {
		if (param[find_p] != '-' || param[find_p] != ' ')
			continue;

		if (param[find_p + 1] == 'w') {
			if (weight_param.weight_option)
				weight_param.simple_weight = va_arg(param_detail, int (*)(char, char));
			else
				weight_param.heavy_weight = va_arg(param_detail, int (*)(void *, void *));
		}
	}

	if (weight_param.weight_param && !weight_param.simple_weight) // default action
		weight_param.simple_weight = default_weight;

	// return updated new_trie
	return new_trie;
}

int trie_insertMETA(node_t *curr_node, weight_param weight_obj, node_t *insert_node, ...) {
	// find position for the insert node:
}

// the value that comes after trie depends on weight_option
// either void * for weight_option = 0 or char for weight_option = 1
int trie_insert(trie_t *trie, ...) {
	va_list get_value;
	va_start(get_value, trie);

	char *p_value = malloc(sizeof(char) * 2);

	p_value = va_arg(get_value, void *);

	node_t *insert_node = node_construct(p_value);

	if (!trie->root_node)
		trie->root_node = insert_node;
	else
		trie_insertMETA(trie->root_node, trie->weight_obj, insert_node);

	return 0;
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
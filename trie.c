#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "trie.h"

// default next for simple_payload
void *default_next(void *payload) {
	if (!((char *) payload)[1])
		return NULL;

	return payload + sizeof(char);
}

// default converter for simple payload
char simple_convert(void *payload) {
	return ((char *) payload)[0];
}

char *singleton_maker(void *payload) {
	char *singleton = malloc(sizeof(char) * 2);

	singleton[0] = ((char *) payload)[0];
	singleton[1] = '\0';

	return singleton;
}

// defualt weight function
int default_weight(char payload1, char payload2) {
	return (int) payload1 < (int) payload2;
}

int default_delete(void *payload) {
	free((char *) payload);

	return 0;
}

typedef struct TrieNode {
	void *payload;

	int thru_weight;
	int end_weight;

	// circularly linked list
	struct TrieNode *next, *prev;
	struct TrieNode *children; // pointer to lower circularly linked list
} node_t;

node_t *node_construct(void *payload) {
	node_t *new_node = malloc(sizeof(node_t));

	new_node->payload = payload;

	new_node->thru_weight = 0;
	new_node->end_weight = 0;

	// circularly link
	new_node->next = new_node;
	new_node->prev = new_node;
	new_node->children = NULL;

	return new_node;
}

// dir = 1 to pull from curr_node->next
// dir = 0 to pull from curr_node->prev
node_t *pull_next(node_t *curr_node, int dir) {
	return dir ? curr_node->next : curr_node->prev;
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
	void *(*next)(void *);

	int (*weight)(struct Weight, void *, void *);
	int (*delete)(void *);

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
		'n': for an insertion (or search), this paramater takes a function
		of type void (*)(void *) and gives the next value in the data type (
		linked list, char *, etc.) -- will set to default (char *) if not inputted
		'd': use for deleting values during destruction or delete

		param will look something like:
			"-w -pc", int (*weight)(char, char)
		or:
			"-w -pv", int (*weight)(void *, void *)
		or:
			"-pc-w-n-d", int (*weight)(char, char), 
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

	new_trie->next = default_next;
	new_trie->weight = major_weight;
	new_trie->delete = default_delete;

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

		weight_scheme.weight_option = param[find_p + 2] == 'c';
	} else
		weight_scheme.weight_option = 1;

	// check for other parameters
	for (find_p = 0; param[find_p + 1]; find_p++) {
		if (param[find_p] != '-' || param[find_p] != ' ')
			continue;

		if (param[find_p + 1] == 'w') {
			if (weight_scheme.weight_option)
				weight_scheme.simple_weight = va_arg(param_detail, int (*)(char, char));
			else
				weight_scheme.heavy_weight = va_arg(param_detail, int (*)(void *, void *));
		} else if (param[find_p + 1] == 'n') {
			new_trie->next = va_arg(param_detail, void *(*)(void *));
		} else if (param[find_p + 1] == 'd') {
			new_trie->delete = va_arg(param_detail, int (*)(void *));
		}
	}

	if (weight_scheme.weight_option && !weight_scheme.simple_weight) // default action
		weight_scheme.simple_weight = default_weight;

	new_trie->weight_obj = weight_scheme;

	// return updated new_trie
	return new_trie;
}

int match(int test1, int test2) {
	return test1 && test2;
}

// 1 for insert right
// 0 for insert left
int inject_node(node_t *curr_node, node_t *new_node, int dir) {
	node_t *curr_next_node = pull_next(curr_node, dir);
	if (dir) {
		curr_node->next = new_node;
		new_node->prev = curr_node;

		curr_next_node->prev = new_node;
		new_node->next = curr_next_node;
	} else {
		curr_node->prev = new_node;
		curr_node->next = curr_node;

		curr_next_node->next = new_node;
		new_node->prev = curr_next_node;
	}

	return 0;
}

int trie_insertMETA(node_t *curr_node, trie_t *meta_func, void *value) {
	// find position for the insert node:
	node_t *start_node = curr_node;

	int weight_ret = meta_func->weight(meta_func->weight_obj, value, curr_node->payload);
	int search_direction = weight_ret > 0 ? 1 : -1, went_full_circ = 0;
	while (weight_ret != 0) {
		if (went_full_circ && start_node == curr_node)
			break;

		went_full_circ = 1;

		// check current position to see if we have found a position to insert
		weight_ret = meta_func->weight(meta_func->weight_obj, value, curr_node->payload);

		if (match(weight_ret > 0, search_direction) || match(weight_ret < 0, !search_direction)) // keep moving in a direction
			curr_node = search_direction ? curr_node->next : curr_node->prev;
		else
			break;
	}

	if (weight_ret != 0) {
		// insert node: right of curr_node if search_direction = 0,
		// left of curr_node if search_direction = 1
		node_t *new_node = node_construct(meta_func->weight_obj.weight_option ?
			singleton_maker(value) : value);

		inject_node(curr_node, new_node, search_direction);
		curr_node = new_node;
	}

	void *get_next_value = meta_func->next(value);

	if (!get_next_value) {
		curr_node->end_weight += 1;
		return 0;
	}

	curr_node->thru_weight++;

	// check for existence of child
	// if no child, need to build one bfore recurring
	if (!curr_node->children)
		curr_node->children = node_construct(meta_func->weight_obj.weight_option ?
			singleton_maker(get_next_value) : get_next_value);

	// recur
	return trie_insertMETA(curr_node->children, meta_func, get_next_value);
}

// the value that comes after trie depends on weight_option
// either void * for weight_option = 0 or char for weight_option = 1
int trie_insert(trie_t *trie, void *p_value) {
	if (!trie->root_node)
		trie->root_node = node_construct(singleton_maker(p_value));

	return trie_insertMETA(trie->root_node, trie, p_value);
}

int trie_destroyMETA(node_t *curr_node, trie_t *meta_func) {
	node_t *start_node = curr_node;

	do {
		if (curr_node->children)
			trie_destroyMETA(curr_node->children, meta_func);

		if (meta_func->delete)
			meta_func->delete(curr_node->payload);

		node_t *next_node = curr_node->next;
		free(curr_node);
		curr_node = next_node;
	} while (start_node != curr_node);

	return 0;
}

int trie_destroy(trie_t *trie) {
	trie_destroyMETA(trie->root_node, trie);

	free(trie);

	return 0;
}
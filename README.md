# trieC
###### Charlie Hall

This trie implementation utilizes a [hashmap](https://github.com/charlie-map/hashmapC) to create a very generalized trie. The trie can store any type of pointer (`void *`). The trie currently (see [future updates](#Future-Updates)] has the functionality:

1. [Creation -- `trie_create()`](#Create)
2. [Insertion -- `trie_insert()`](#Insert)
3. [Search -- `trie_search()`](#Search)
4. [Destroy -- `trie_destroy()`](#Destroy)

*Have any suggestions or update requests? Please create an issue.*
*See `example.c` for more on how each of the above functions work.*

# Create
Create makes a new header for a trie. The header stores information related to the trie and a pointer to the starting trie hashmap. The command uses the following pattern:
```C
trie_t *trie_create(char *param, ...);
```

The single parameter `char *param` helps define the type of data the trie will be storing. There are currently four arguments that can be used:

1. `-pc` or `-pv`: This defines the type of data to be stored. Using `-pc` means the trie is using a `char` at each level, and `-pv` means there is a `void *` stored instead. *Note*: a `-pc` trie system will still store a `char *`, however, this cannot be seen and will not affect the utilization of the program. If `-pc` is given, none of the following parameters are required.
2. `-c`: A comparer function. This should return 1 if the two values are the same, and 0 otherwise. The function should have the form:
```C
int (*)(void *, void *);
```

3. `-n`: The next function is used for the insertion step. If a [linked list](https://github.com/charlie-map/linkedlistC) is used in `-pv`, then the next function should return the following pointer in the list. This function should have the form:
```C
void *(*)(void *);
```
The next function requires a way to return `NULL` when the end of the value is reached. So if an `int **` is used, the final position should have some way of recognizing a terminator and stopping the continuation.

4. `-d`: A delete function. This goes through the trie and deletes all payloads at every level. This has the form:
```C
int (*)(void *);
```

### Using parameters
So each input for `param` will alter how the rest of the function inputs look. If `-pc` is used, the function will just be:
```C
trie_t *my_trie = trie_create("-pc");
```
However, if `-pv` is used, `-c`, `-n`, and `-d` are necessary, so `param` will look like:
```C
trie_t *my_trie = trie_create("-pv-c-n-d", int (*)(void *, void *), void *(*)(void *), int (*)(void *));
```
***Note***: the ordering and spacing of the commands does not matter as long as the function pointers inputted line up with the, so the previous line could also look like:
```C
trie_t *my_trie = trie_create("-d -c-n  -pv", int (*)(void *), int (*)(void *, void *), void *(*)(void*));
```

# Insert
Insert takes a pointer (a `char *` for `-pc`) and seeks a position in the trie. At each branch level it will check to see if that current payload is there. If it is not, then a new node is created and inserted into the hashmap. The follows the pattern of first the meta data of the trie (a `trie_t *`) and then the value to be inserted (a pointer that follows the previously defined functions in creation of the meta `trie_t`). This has the form:
```C
int trie_insert(trie_t *trie, void *p_value);
```

# Search
Search has the same pattern as insert. However, this returns the weight of the inputted `void *` search value. If `0` is returned, the search term was not found. This will have a similar form to insert:
```C
int trie_search(trie_t *trie, void *p_value);
```

# Destroy
Destroy goes through all levels of the trie and wipes all of the data. If a delete function was given during [creation](#Create), then each of the payloads will also be freed. This just takes in the meta header:
```C
int trie_destroy(trie_t *trie);
```

# Future Updates
Currently no future enhancements are planned.

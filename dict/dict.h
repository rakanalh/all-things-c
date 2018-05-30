#define DICT_INITIAL_CAPACITY 101

#define dict_foreach(dict, entry)

typedef struct dict_entry {
	char *key;
	void *value;
	struct dict_entry* next;
} dict_entry;

typedef struct {
	unsigned long count;
	dict_entry **values;
	dict_entry *current;
} dict;

dict* dict_new();

void dict_free(dict* dict);

void dict_set(dict* dict, char* key, void* value);

void* dict_get(dict* dict, char* key);

void dict_del(dict* dict, char* key);

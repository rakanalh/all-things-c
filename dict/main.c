#include <stdio.h>
#include <stdint.h>
#include "dict.h"

int main(int argc, char** argv) {
	dict* dict = dict_new();

	printf("Setting values\n");
	dict_set(dict, "name", "Rakan");
	dict_set(dict, "age", (void *) (intptr_t) 15);

	printf("Reading values\n");

	dict_entry* name = dict_get(dict, "name");
	dict_entry* age = dict_get(dict, "age");

	if(name != NULL) {
		printf("Name is: %s\n", (char*) name->value);
	}

	if(age != NULL) {
		printf("Age is: %ld\n", (intptr_t) age->value);
	}

	dict_entry* entry = NULL;
	dict_foreach(dict, entry) {
		printf("key is %s\n", entry->key);
	}

	printf("Deleting values\n");
	dict_del(dict, "age");
	printf("Clear dict\n");
	dict_free(dict);
	return 0;
}

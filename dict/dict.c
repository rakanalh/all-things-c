#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "dict.h"

static unsigned long hash_key(char* key) {
	unsigned long hash = 5381;
	int c;
	while((c = *key++) != 0) {
		hash = ((hash << 5) + hash) + c;
	}
	return hash % DICT_INITIAL_CAPACITY;
}

static dict_entry* create_entry(char* key, void* value) {
	dict_entry* entry = (dict_entry *) malloc(sizeof(dict_entry));
	entry->key = key;
	entry->value = value;
	entry->next = NULL;
	return entry;
}

static dict_entry* dict_entry_search(dict_entry* head, char* key) {
	if(head == NULL) return NULL;
	for(dict_entry* current = head; current; current = current->next) {
		if(strcmp(current->key, key) == 0) {
			return current;
		}
	}
	return NULL;
}

static void dict_entry_free(dict_entry* head) {
	if(head == NULL) {
		return;
	}
	dict_entry *current, *next;
	for(current = head; current;) {
		next = current->next;
		free(current);
		current = next;
	}
}

dict* dict_new() {
	dict *dict = malloc(sizeof(dict));
	dict->values = malloc(sizeof(void*) * DICT_INITIAL_CAPACITY);
	for(int i = 0; i < DICT_INITIAL_CAPACITY; i++) {
		dict->values[i] = NULL;
	}
	dict->count = 0;
	dict->current = NULL;
	return dict;
}
void dict_free(dict* dict) {
	for(int i = 0; i < DICT_INITIAL_CAPACITY; i++) {
		if(dict->values[i] != NULL) {
			dict_entry_free(dict->values[i]);
		}
	}
	free(dict->values);
	free(dict);
}

void dict_set(dict* dict, char* key, void* value) {
	unsigned long index = hash_key(key);
	dict->count++;
	dict_entry *head = dict->values[index];
	if(head == NULL) {
		dict->values[index] = create_entry(key, value);
		return;
	}

	for(head = dict->values[index]; head; head = head->next) {
		if(strcmp(head->key, key) == 0) {
			head->value = value;
			return;
		}
		if(head->next == NULL) {
			head->next = create_entry(key, value);
		}
	}
}

void* dict_get(dict* dict, char* key) {
	unsigned long index = hash_key(key);
	return dict_entry_search(dict->values[index], key);
}

void dict_del(dict* dict, char* key) {
	unsigned long index = hash_key(key);
	dict_entry_free(dict->values[index]);
	dict->values[index] = NULL;
}
